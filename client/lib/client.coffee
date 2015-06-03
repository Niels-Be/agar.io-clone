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

	player: 
		x: 0
		y: 0
		size: 0

	elements: {}	

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

		@lobbySocket.on "roomCreated", (status, err) =>
			if status
				@lobbySocket.emit "getRooms"
			else
				console.log("Room Create:", err)


		#stat the main loop
		@loop()

	#setup callbacks on the socket
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

		@socket.on "setElements", (objs) =>
			#just replace the entiere list
			@elements = {}
			for o in objs
				@elements[o.id] = new Ball(@options[o.type], o)

		@socket.on "updateElements", (objs) =>
			for o in objs.created
				@elements[o.id] = new Ball(@options[o.type], o)
			for o in objs.deleted
				delete @elements[o]
			for o in objs.positions
				@elements[o.id].updateData(o)
			#a bit hacky but faster then checking every moveable
			if @player.balls
				for b in @player.balls when @elements.hasOwnProperty(b)
					@elements[b].options = @options.player

		@socket.on "updatePlayer", (@player) =>
			@massText.innerHTML = "Mass: "+@player.mass
			@updatePlayer()

		@socket.on "rip", =>
			@gameStarted = false
			spawnbox.hidden = false
			@lobbySocket.emit "getRooms"

		@socket.on "disconnect", =>
			console.log("Disconnected")
			@socket.close();
			@inRoom = false;

		@socket.on "error", (err) =>
			console.log("Error:",err)

		@socket.on "stats", (stats) =>
			console.log("Stats", stats)


	#initialize the window
	init: ->
		@canvas = document.getElementById("cvs")

		@canvas.addEventListener "mousemove", (evt) =>
			@target.x = evt.clientX - @screen.width  / 2;
			@target.y = evt.clientY - @screen.height / 2;
		@canvas.addEventListener "keypress", (evt) =>
			#console.log("Key:", evt.keyCode, evt.charCode)
			@socket.emit "splitUp", @target if evt.charCode == 32
			@socket.emit "shoot", @target if evt.charCode == 119
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
			@socket.emit "leave"
			@socket.disconnect()
			@inRoom = false
			setTimeout( (=> @join(index)), 10)
			return

		console.log("Connecting ...")
		#check whether we have an existing connection to that room
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

	#Swtich from Spectater mode into Game mode
	#Join should be called before
	start: ->
		console.log("Starting Game")
		@name = @nameText.value
		@socket.emit "start", @name

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
			@lastTick = now
			@loop()

	#pre calculate positions of the next frame to reduce lag
	update: (timediff) ->
		for i,m of @elements
			m.update timediff
		@updatePlayer()
		@socket.emit "updateTarget", @target

		@fpstimer += timediff
		@fps++
		if @fpstimer > 1
			@statusText.innerHTML = "FPS:" + @fps
			@fps = 0
			@fpstimer = 0

	#update camera position
	updatePlayer: ->
		if @gameStarted
			#center the camera in the middle of all balls
			@player.x = @player.y = @player.size = 0
			for b in @player.balls when @elements.hasOwnProperty(b)
				@player.size += @elements[b].size
			for b in @player.balls when @elements.hasOwnProperty(b)
				@player.x += @elements[b].x * @elements[b].size
				@player.y += @elements[b].y * @elements[b].size
			@player.x /= @player.size 
			@player.y /= @player.size 
			@player.size /= @player.balls.length

		else #show the entiere map
			@player.x = @gamefield.width / 2
			@player.y = @gamefield.height / 2
			@player.size = Math.log(Math.min(@gamefield.width - @screen.width, @gamefield.height - @screen.height)) / @options.viewPortScaleFactor / 2

	#render everything
	render: ->
		#Clear Canvas
		@graph.setTransform(1,0,0,1,0,0);
		@graph.fillStyle = @options.backgroundColor;
		@graph.fillRect(0, 0, @screen.width, @screen.height);

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


		@grid.render(@graph)
		
		for i,m of @elements
			m.render(@graph)

	#debug method to get performance information from the server
	getStats: ->
		@socket.emit "getStats"
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



