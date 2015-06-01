class Player

	constructor: (@socket, @name, @color, @room) ->
		@socket.on "splitUp", @splitUp.bind(@)
		@socket.on "shoot", @shoot.bind(@)
		@socket.on "updateTarget", @setTarget.bind(@)
		#TODO Chat
		@balls = [
			@room.createBall(@)
		]
		@mass = 0

	splitUp: (target) ->
		for ball in @balls when ball.mass >= @room.options.player.minSpitMass
			@balls.push ball.splitUp target
		#console.log("Player "+@name+" split")
		@updateMass()

	shoot: (target) ->
		deg = Math.atan2(target.y, target.x)
		for ball in @balls when ball.mass > @room.options.shoot.mass*2
			b = @room.createShoot(ball)
			ball.addMass -@room.options.shoot.mass
			#TODO check border
			b.x += ball.size*2 * Math.cos(deg)
			b.y += ball.size*2 * Math.sin(deg)
			b.setTarget target
		@updateMass()

	setTarget: (@target) ->
		for ball in @balls
			t = #TODO something is still wrong here
				#     maybe player pos on server and client differ
				x: @target.x - (ball.x - @x)
				y: @target.y - (ball.y - @y)
			ball.setTarget(t)

	removeBall: (ball) ->
		for i,b of @balls
			@balls.splice(i,1) if b.id == ball.id
		if @balls.length == 0
			console.log("Player "+@name + " got killed")
			@socket.emit "rip"
		@updateMass()

	update: (timediff) ->
		@x = @y = 0
		for ball in @balls
			@x += ball.x / @balls.length
			@y += ball.y / @balls.length

	updateMass: ->
		@mass = 0
		for ball in @balls
			@mass += ball.mass
		#if @mass > @room.options.player.maxMass && @room.options.player.maxMass != 0
		#	@splitUp @target
		#	@room.destroyElement @ball[0]
		@socket.emit "updatePlayer", @get()

	get: ->
		{
			name: @name
			mass: @mass
			balls: (b.id for b in @balls)
		}