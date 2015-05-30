class Player

	constructor: (@socket, @name, @room) ->
		@socket.on "splitUp", @splitUp.bind(@)
		@socket.on "shoot", @shoot.bind(@)
		@socket.on "updateTarget", @setTarget.bind(@)
		#TODO Chat
		@balls = [
			@room.createBall(@)
		]
		@mass = 0

	splitUp: (target) ->
		deg = Math.atan2(target.y, target.x)
		vel = 
			x: @room.options.shoot.speed * Math.cos(deg)
			y: @room.options.shoot.speed * Math.sin(deg)

		for ball in @balls when ball.mass >= @room.options.player.minSpitMass
			b = @room.createBall(@)
			b.setMass Math.floor(ball.mass / 2)
			ball.setMass Math.floor(ball.mass / 2)
			b.x = ball.x + ball.size*2 * Math.cos(deg)
			b.y = ball.y + ball.size*2 * Math.sin(deg)
			b.setTarget target
			b.setBoost vel, @room.options.shoot.acceleration
			
			@balls.push b
		#console.log("Player "+@name+" split")
		@updateMass()

	shoot: (target) ->
		deg = Math.atan2(target.y, target.x)
		for ball in @balls when ball.mass > @room.options.shoot.mass*2
			b = @room.createShoot(ball)
			ball.addMass -@room.options.shoot.mass
			b.x += ball.size*2 * Math.cos(deg)
			b.y += ball.size*2 * Math.sin(deg)
			b.setTarget target
		@updateMass()

	setTarget: (target) ->
		for ball in @balls
			t =
				x: target.x - (ball.x - @x)
				y: target.y - (ball.y - @y)
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
		@socket.emit "updatePlayer", @get()

	get: ->
		{
			name: @name
			mass: @mass
			balls: (b.id for b in @balls)
		}