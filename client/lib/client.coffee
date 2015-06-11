class Game
	net: new Network("ws://"+window.location.host+"/websocket/")

	screen:
		width: window.innerWidth
		height: window.innerHeight

	view:
		width: window.innerWidth
		height: window.innerHeight

	gameStarted: false
	inRoom: false

	@ElementTypes:
		0: "ball"
		1: "food"
		2: "shoot"
		3: "obstracle"
		4: "item"

	@defaultOptions:
		viewPortScaleFactor: 0.01
		backgroundColor: '#EEEEEE'

		grid:
			lineColor: '#DADADA'
			borderColor: '#A0A0A0'
			borderSize: 5
			size: 50

		food:
			border: 2,
			borderColor: "#f39c12"
			fillColor: "#f1c40f"
			size: 5

		obstracle:
			border: 5
			borderColor: "#006600"
			fillColor: "#00FF00"

		shoot:
			border: 2
			borderColor: "#f39c12"
			fillColor: "#FF0000"

		player:
			border: 5
			borderColor: "#FFFFCC"
			fillColor: "#ea6153" #fallback color
			textColor: "#FFFFFF"
			textBorder: "#000000"
			textBorderSize: 3
			defaultSize: 10

		ball:
			border: 5
			borderColor: "#CC0000"
			fillColor: "#c0392b" #fallback color
			textColor: "#FFFFFF"
			textBorder: "#000000"
			textBorderSize: 3
			defaultSize: 10

		item:
			border: 5
			borderColor: "#6666FF"
			fillColor: "#0000FF"

	player: 
		x: 0
		y: 0
		size: 0
		mass: 0
		balls: {}

	elements: {}	

	target:
		x: 0
		y: 0
		changed: false

	fps: 0
	fpstimer: 0
	lastTick: 0

	name: "NoName"
	rooms: {}

	constructor: (options) ->
		@options = extend (extend {}, Game.defaultOptions), options
		@grid = new Grid(@, @options.grid)
		@init()


		#@lobbySocket.on "roomCreated", (status, err) =>
		#	if status
		#		@lobbySocket.emit "getRooms"
		#	else
		#		console.log("Room Create:", err)


		#setup callbacks on the socket

		@net.onConnect =>
			console.log("Connected")
			@net.emit Network.Packets.GetLobby

		@net.on Network.Packets.GetLobby, (packet) =>
			@rooms = {}
			if not @lastRoom
				@lastRoom = packet.data[packet.data.length-1].id
			for room in packet.data
				@rooms[room.id] = room
			console.log("Rooms:", @rooms)
			@roomsText.innerHTML = ""
			for i,room of @rooms
				@roomsText.innerHTML += "<option value=\""+room.id+"\" "+(if @lastRoom == room.id then "selected=\"selected\"" else "")+">"+room.name+" ("+room.playerCount+")</option>"
			@join @lastRoom

		@net.on Network.Packets.Start, =>
			console.log("Game Started")
			@gameStarted = true

		@net.on Network.Packets.SetElements, (packet) =>
			#just replace the entiere list
			@elements = {}
			for o in packet.elements
				@elements[o.id] = new Ball(@options[Game.ElementTypes[o.type]], o)
				if(o.type == 0 && @player.balls.hasOwnProperty(o.id))
					@elements[o.id].options = @options.player
					@player.balls[o.id] = @elements[o.id]

		@net.on Network.Packets.UpdateElements, (packet) =>
			#Ignore all errors
			try
				for o in packet.newElements
					@elements[o.id] = new Ball(@options[Game.ElementTypes[o.type]], o)
					if(o.type == 0 && @player.balls.hasOwnProperty(o.id))
						@elements[o.id].options = @options.player
						@player.balls[o.id] = @elements[o.id]
						#console.log("PlayerBall", @elements[o.id])
			catch err
			try
				for o in packet.deletedElements
					delete @elements[o]
			catch err
			try
				for o in packet.updateElements
					@elements[o.id].updateData(o)
					#console.log("Update", @elements[o.id].x, @elements[o.id].y)
			catch err

		@net.on Network.Packets.PlayerUpdate, (packet) =>
			@player.mass = packet.mass
			@player.balls = {}
			for b in packet.balls
				if @elements.hasOwnProperty(b)
					@player.balls[b] = @elements[b]
					@elements[b].options = @options.player
					#console.log("PlayerBall", @elements[b])
				else
					@player.balls[b] = null
			#console.log(@player, packet)
			@massText.innerHTML = "Mass: "+@player.mass
			@updatePlayer()

		@net.on Network.Packets.RIP, =>
			console.log("You got killed")
			@gameStarted = false
			spawnbox.hidden = false
			#@lobbySocket.emit "getRooms"

		@net.onDisconnect =>
			console.log("Disconnected")
			@inRoom = false;

		@net.on Network.Packets.GetStats, (packet) =>
			console.log("Stats", packet)



		#stat the main loop
		@loop()

	#initialize the window
	init: ->
		@canvas = document.getElementById("cvs")

		@canvas.addEventListener "mousemove", (evt) =>
			@target.x = evt.clientX - @screen.width  / 2;
			@target.y = evt.clientY - @screen.height / 2;
			@target.changed = true
		@canvas.addEventListener "keypress", (evt) =>
			#console.log("Key:", evt.keyCode, evt.charCode)
			@net.emit Network.Packets.SplitUp if evt.charCode == 32
			@net.emit Network.Packets.Shoot if evt.charCode == 119
			@getStats() if evt.charCode == 100
		#TODO add touch events

		@canvas.width = @screen.width
		@canvas.height = @screen.height

		window.addEventListener "resize", =>
			@screen.width = window.innerWidth
			@screen.height = window.innerHeight
			@canvas.width = window.innerWidth
			@canvas.height = window.innerHeight

		@graph = @canvas.getContext "2d"

		@massText = document.getElementById("mass")
		@statusText = document.getElementById("status")
		@roomsText = document.getElementById("rooms")
		@nameText = document.getElementById("name")
		@spawnbox = document.getElementById("spawnbox")

	#Joins a room
	#after this call we are inside a room and recive position updates from the server
	#but we are in spectater mode
	join: (index) ->
		console.log("Joining Room " + @rooms[index].name)
		@gamefield = @rooms[index].options
		#only disconnect if we switch the room
		if @inRoom && index != @lastRoom
			@net.emit Network.Packets.Leave
			@inRoom = false
			setTimeout( (=> @join(index)), 10)
			return

		console.log("Connecting ...")

		@net.emit new JoinPacket(index)
		@inRoom = true
		@lastRoom = index
		@updatePlayer()

	#Swtich from Spectater mode into Game mode
	#Join should be called before
	start: ->
		console.log("Starting Game")
		@name = @nameText.value
		@net.emit new StartPacket(@name)

		#Hide panel and set focus to canvas
		spawnbox.hidden = true
		@canvas.focus() #TODO this call does not work

	#Main loop of the game
	loop: ->
		window.requestAnimationFrame (now) =>
			timediff = now - @lastTick
			if @inRoom
				@update timediff * 1e-3
				@render()
			else
				@statusText.innerHTML = "FPS: -"
			@lastTick = now
			@loop()

	#pre calculate positions of the next frame to reduce lag
	update: (timediff) ->
		for i,m of @elements
			m.update timediff
		if @gameStarted
			@updatePlayer()
			if @target.changed
				@net.emit new TargetPacket(@target.x, @target.y)
				@target.changed = false

		@fpstimer += timediff
		@fps++
		if @fpstimer >= 1
			@statusText.innerHTML = "FPS:" + @fps
			@fps = 0
			@fpstimer = 0

	#update camera position
	updatePlayer: ->
		if @gameStarted
			#center the camera in the middle of all balls
			@player.x = @player.y = @player.size = 0
			i = 0
			for j,b of @player.balls when b != null
				@player.x += b.x * b.size
				@player.y += b.y * b.size
				@player.size += b.size
				i++
			if(i > 0)
				@player.x /= @player.size 
				@player.y /= @player.size 
				@player.size /= i

		else #show the entiere map
			@player.x = @gamefield.width / 2
			@player.y = @gamefield.height / 2
			@player.size = Math.log(Math.min(@gamefield.width - @screen.width, @gamefield.height - @screen.height)) / @options.viewPortScaleFactor / 2

	#render everything
	render: ->
		#Clear Canvas
		#@graph.setTransform(1,0,0,1,0,0);
		#@graph.fillStyle = @options.backgroundColor;
		#@graph.fillRect(0, 0, @screen.width, @screen.height);

		#Setup Camera
		scale = 1 / (@options.viewPortScaleFactor * @player.size + 1)
		
		#We always draw the entiere gamefield and only move the camera arround
		#this prevents a lot calculations
		@graph.setTransform(
			scale,		#scale x
			0, 			#rotate x
			0, 			#rotae y
			scale, 		#scale y
			@screen.width / 2 - @player.x * scale,  #translate x
			@screen.height / 2 - @player.y * scale  #translate y
		)

		viewPort =
			left: @player.x - @screen.width / 2 / scale
			top: @player.y - @screen.height / 2 / scale
			right: @player.x + @screen.width / 2 / scale
			botom: @player.y + @screen.height / 2 / scale 

		#Clear Canvas
		@graph.fillStyle = @options.backgroundColor;
		@graph.fillRect(viewPort.left, viewPort.top, @screen.width / scale, @screen.height / scale);

		@grid.render(@graph)
		
		for i,m of @elements
			if (m.x + m.size > viewPort.left &&
					m.y + m.size > viewPort.top &&
					m.x - m.size < viewPort.right &&
					m.y - m.size < viewPort.botom )

				m.render(@graph)

	#debug method to get performance information from the server
	getStats: ->
		@net.emit Network.Packets.GetStats
		return

	createRoom: (name, options) ->
		@lobbySocket.emit "createRoom", name, options
		return


#setup requestAnimationFrame to work in every environment
`(function() {
    var lastTime = 0;
    var vendors = ['webkit', 'moz'];
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
        window.cancelAnimationFrame =
          window[vendors[x]+'CancelAnimationFrame'] || window[vendors[x]+'CancelRequestAnimationFrame'];
    }

    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function() { callback(currTime + timeToCall); },
              timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };

    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        };
}());`

#create the game
#you have acess to the meber variables throug game.*
#in productive version prevent this by removing the @
@game = new Game()



