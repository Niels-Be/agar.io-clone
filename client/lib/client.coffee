class Game
	lobbySocket: io()
	socket: null

	screen:
		width: window.innerWidth
		height: window.innerHeight

	view:
		width: window.innerWidth
		height: window.innerHeight

	gameStarted: false
	inRoom: false

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
			borderColor: "#f39c12",
			fillColor: "#f1c40f",
			size: 5

		player:
			border: 3,
			borderColor: "#c0392b",
			fillColor: "#ea6153",
			textColor: "#FFFFFF",
			textBorder: "#000000",
			textBorderSize: 3,
			defaultSize: 10

		enemy:
			border: 3,
			borderColor: "#27ae60",
			fillColor: "#2ecc71",
			textColor: "#FFFFFF",
			textBorder: "#000000",
			textBorderSize: 3,
			defaultSize: 10

	player: 
		x: 0
		y: 0
		size: 0

	elements:
		static: {}
		moveable: {}	

	target:
		x: 0
		y: 0

	fps: 0
	fpstimer: 0
	lastTick: 0

	name: "NoName"
	rooms: {}
	lastRoom: 0

	constructor: (options) ->
		@options = extend (extend {}, Game.defaultOptions), options
		@grid = new Grid(@, @options.grid)
		@init()

		@lobbySocket.emit "getRooms"

		@lobbySocket.on "availableRooms", (rooms) =>
			@rooms = extend rooms, @rooms
			console.log("Rooms:", @rooms)
			@roomsText.innerHTML = ""
			for i,room of @rooms
				@roomsText.innerHTML += "<option value=\""+i+"\" "+(if @lastRoom == i then "selected=\"selected\"" else "")+">"+room.name+" ("+room.playercount+")</option>"
			@join @lastRoom


		@loop()

	setCallbacks: (@socket) ->
		@socket.on "connect", =>
			console.log("Connected")
		@socket.on "reconnect", =>
			console.log("Reconnected")

		@socket.on "playerJoined", (ply) =>
			console.log("A player joind:", ply)

		@socket.on "start", (@player) =>
			@updatePlayer()
			@gameStarted = true

		@socket.on "createFood", (food) =>
			@elements.static[food.id] = new Ball(@options.food, food)

		@socket.on "deleteElement", (id) =>
			delete @elements.static[id] if @elements.static.hasOwnProperty(id)
			delete @elements.moveable[id] if @elements.moveable.hasOwnProperty(id)

		@socket.on "updateStatics", (objs) =>
			@elements.static = {}
			for o in objs
				@elements.static[o.id] = new Ball(@options.food, o)

		@socket.on "updateMoveables", (objs) =>
			@elements.moveable = {}
			for o in objs
				@elements.moveable[o.id] = new Ball(@options.enemy, o)

		@socket.on "updatePlayer", (@player) =>
			@massText.innerHTML = "Mass: "+@player.mass
			@updatePlayer()

		@socket.on "rip", =>
			@gameStarted = false
			spawnbox.hidden = false
			@lobbySocket.emit "getRooms"

		@socket.on "connect_failed", =>
			console.log("Connect Failed")
			@socket.close();
			@inRoom = false;

		@socket.on "disconnect", =>
			console.log("Disconnected")
			@socket.close();
			@inRoom = false;

		@socket.on "error", (err) =>
			console.log("Error:",err)


	init: ->
		@canvas = document.getElementById("cvs")
		@canvas.addEventListener "mousemove", (evt) =>
			@target.x = evt.clientX - @screen.width  / 2;
			@target.y = evt.clientY - @screen.height / 2;
		@canvas.addEventListener "keypress", (evt) =>
			console.log("Key:", evt.keyCode, evt.charCode)
			@socket.emit "splitUp", @target if evt.charCode == 32
			@socket.emit "shoot", @target if evt.charCode == 119
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


	join: (index) ->
		console.log("Joining Room " + @rooms[index].name)
		@gamefield = @rooms[index].options
		if @inRoom && index != @lastRoom
			@socket.emit "leave"
			@socket.disconnect()
			@inRoom = false
			setTimeout( (=> @join(index)), 10)
			return

		console.log("Connecting ...")
		if @rooms[index].socket
			@rooms[index].socket.connect() unless @rooms[index].socket.connected
			@socket = @rooms[index].socket
		else
			@rooms[index].socket = io.connect('/'+@rooms[index].name)
			@setCallbacks(@rooms[index].socket)
		@socket.emit "join"
		@inRoom = true
		@lastRoom = index
		@updatePlayer()

	start: ->
		console.log("Starting Game")
		@name = @nameText.value
		@socket.emit "start", @name
		spawnbox.hidden = true
		@canvas.focus()


	loop: ->
		window.requestAnimationFrame (now) =>
			timediff = now - @lastTick
			if @inRoom
				@update timediff * 1e-3
				@render()
			@lastTick = now
			@loop()

	update: (timediff) ->
		for i,m of @elements.moveable
			m.update timediff
		@updatePlayer()
		@socket.emit "updateTarget", @target

		@fpstimer += timediff
		@fps++
		if @fpstimer > 1
			@statusText.innerHTML = "FPS:" + @fps
			@fps = 0
			@fpstimer = 0

	updatePlayer: ->
		if @gameStarted
			@player.x = @player.y = @player.size = 0
			for b in @player.balls when @elements.moveable.hasOwnProperty(b)
				@player.x += @elements.moveable[b].x / @player.balls.length
				@player.y += @elements.moveable[b].y / @player.balls.length
				@player.size += @elements.moveable[b].size / @player.balls.length
		else
			@player.x = @gamefield.width / 2
			@player.y = @gamefield.height / 2
			@player.size = Math.log(Math.min(@gamefield.width - @screen.width, @gamefield.height - @screen.height)) / @options.viewPortScaleFactor / 2

	render: ->
		#Clear Canvas
		@graph.setTransform(1,0,0,1,0,0);
		@graph.fillStyle = @options.backgroundColor;
		@graph.fillRect(0, 0, @screen.width, @screen.height);

		#Set viewport
		scale = 1 / (@options.viewPortScaleFactor * @player.size + 1)

		@graph.setTransform(
			scale, 
			0, 
			0, 
			scale, 
			@screen.width / 2 - @player.x * scale,
			@screen.height / 2 - @player.y * scale )

		@grid.render(@graph)
		
		for i,s of @elements.static
			s.render(@graph)
		for i,m of @elements.moveable
			m.render(@graph) #unless m == @player

		#@player.render(@graph)


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

@game = new Game()



