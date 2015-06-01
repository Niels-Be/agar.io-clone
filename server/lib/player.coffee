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
			t = #relativ direction to camera and player
				x: @target.x - (ball.x - @x)
				y: @target.y - (ball.y - @y)
			@balls.push ball.splitUp t
		#console.log("Player "+@name+" split")
		@updateMass()

	shoot: (target) ->
		for ball in @balls when ball.mass > @room.options.shoot.mass*2
			t = #relativ direction to camera and player
				x: @target.x - (ball.x - @x)
				y: @target.y - (ball.y - @y)
			b = @room.createShoot(ball, t)

		@updateMass()

	setTarget: (@target) ->
		for ball in @balls
			t = #relativ direction to camera and player
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
		@x = @y = @size = 0
		for b in @balls
			@size += b.size
		for b in @balls
			@x += b.x * b.size
			@y += b.y * b.size
		@x /= @size 
		@y /= @size
		@size /= @balls.length

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