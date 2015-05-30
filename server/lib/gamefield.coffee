class Gamefield

	@defaultOptions:
		width: 5000
		height: 5000
		food:
			color: '#f1c40f'
			spawn: 5 	#per Sec
			max: 500
			mass: 1
			size: 5
		player:
			color: ['#EA6153', '#00FFFF', '#7FFF00', '#6495ED', '#9932CC', '#FF00FF', '#FFE4B5', '#000000']
			size: 15
			force: 50
			acceleration: 1000
			speed: 300
			speedPenalty: 0.005
			eatFactor: 1.2
			minSpitMass: 20
		shoot:
			mass: 10
			speed: 750
			acceleration: 400

	constructor: (@name, options) ->
		@options = extend (extend {}, Gamefield.defaultOptions), options
		@room = io.of('/'+@name)

		console.log("Created Room "+@name)

		@player =  
			length: 0
		
		@elements = 
			static: []
			moveable: []
			id: 1

		@foodSpawnTimer = 0
		@foodCount = 0

		@playerUpdateTimer = process.hrtime()

		for [0..@options.food.max/2]
			@createFood()
		@update 0

		@room.on "connection", (socket) =>
			console.log("Client connected to room " + @name)

			socket.on "join", =>
				console.log("Client is ready")
				socket.emit "updateMoveables", (s.get() for s in @elements.moveable)
				socket.emit "updateStatics", (s.get() for s in @elements.static)

			socket.on "leave", =>
				console.log("Client left room "+@name)
				socket.disconnect()

			socket.on "start", (name) =>
				console.log("Player "+name+" joind the game "+@name)
				color = @options.player.color[Math.round(Math.random()*@options.player.color.length)]
				ply = new Player(socket, name, color, @)
				@player[socket.id] = ply
				@player.length++
				@room.emit "playerJoined", ply.get()
				socket.emit "start", ply.get()

			socket.on "disconnect", =>
				if @player.hasOwnProperty socket.id
					console.log("Player "+@player[socket.id].name+" disconnected")
					for b in @player[socket.id].balls
						for i,e of @elements.moveable
							@elements.moveable.splice(i, 1) if e.id == b.id
					delete @player[socket.id]
					@player.length--


	generatePos: ->
		[Math.floor(Math.random() * @options.width), Math.floor(Math.random() * @options.height)]

	createFood: ->
		@foodCount++
		[x, y] = @generatePos()
		f = new Food(@, x, y)
		f.id = @elements.id++
		@elements.static.push f
		f

	createBall: (player) ->
		[x, y] = @generatePos()
		b = new Ball(@, x, y, player.color, player, @options.player.size, @options.player.speed)
		b.id = @elements.id++
		@elements.moveable.push b
		b

	createShoot: (ball) ->
		b = new Shoot(@, ball.x, ball.y, ball.color)
		b.id = @elements.id++
		@elements.moveable.push b
		b

	destroyElement: (elem) ->
		if elem instanceof StaticElement
			@foodCount-- if elem instanceof Food
			for i,e of @elements.static
				@elements.static.splice(i, 1) if e.id == elem.id	
		else if elem instanceof MoveableElement
			elem.player.removeBall(elem) if elem.player && elem instanceof Ball
			for i,e of @elements.moveable
				@elements.moveable.splice(i, 1) if e.id == elem.id
		else
			throw "Element is not of type Element"
		@room.emit "deleteElement", elem.id

	update: (timediff) ->
		@playerUpdateTimer = process.hrtime()

		for elem in @elements.moveable
			elem.update timediff

		destoryLater = []
		for i,elem1 of @elements.moveable
			for elem2 in @elements.static
				if elem1.intercept elem2
					if elem1.canEat elem2
						elem1.addMass elem2.mass
						@destroyElement elem2
						elem1.player.updateMass() if elem1.player
						break
			for elem2 in @elements.moveable
				if elem1.id != elem2.id && elem1.intercept elem2
					if elem1.canEat elem2
						#console.log(elem1.id,"eats", elem2.id)
						elem1.addMass elem2.mass
						destoryLater.push elem2
						elem1.player.updateMass() if elem1.player
					else if elem2.canEat elem1
						#console.log(elem2.id,"eats", elem1.id)
						elem2.addMass elem1.mass
						destoryLater.push elem1
						elem2.player.updateMass() if elem2.player
					break

		@destroyElement e for e in destoryLater

		for id, ply of @player when id != "length"
			ply.update timediff

		@foodSpawnTimer += timediff
		if @foodSpawnTimer > 1 / @options.food.spawn
			if @foodCount < @options.food.max
				f = @createFood()
				@room.emit "createFood", f.get()
			@foodSpawnTimer = 0

		@room.emit "updateMoveables", (m.get() for m in @elements.moveable)

		diff = process.hrtime(@playerUpdateTimer)
		setTimeout( =>
			diff2 = process.hrtime(@playerUpdateTimer)
			@update(diff2[0] + diff2[1] * 1e-9)
		, 1000 / 60 - (diff[0]*1e3 + diff[1]*1e-6))


	get: ->
		{
			name: @name,
			playercount: @player.length
			options: @options
		}
