class Ball extends MoveableElement

	constructor: (@gamefield, @x, @y, @color, @player, @size = 1, @speed = 100) ->
		super(@gamefield, @x, @y, @color, @size, @speed)
		@setMass(20)

	setMass: (mass) ->
		@mass = mass
		#TODO look for better functions exponential is to hard
		@size = @gamefield.options.player.size + 150 * Math.log((mass+150)/150)
		@speed = @gamefield.options.player.speed * Math.exp(-@gamefield.options.player.speedPenalty * mass)
	addMass: (mass) ->
		@setMass(@mass + mass)

	canEat: (other) ->
		other.size * @gamefield.options.player.eatFactor < @size

	splitUp: (target) ->
		deg = Math.atan2(target.y, target.x)
		vel = 
			x: @gamefield.options.shoot.speed * Math.cos(deg)
			y: @gamefield.options.shoot.speed * Math.sin(deg)

		b = @gamefield.createBall(@player)
		b.setMass Math.floor(@mass / 2)
		@setMass Math.floor(@mass / 2)
		#TODO check border
		b.x = @x + @size*1.6 * Math.cos(deg)
		b.y = @y + @size*1.6 * Math.sin(deg)
		b.setTarget target
		b.setBoost vel, @gamefield.options.shoot.acceleration
		b

	get: ->
		extend(super(), {
			name: @player.name if @player
			mass: @mass
			type: "ball"
		})


class Shoot extends Ball

	constructor: (@gamefield, @x, @y, @color) ->
		super(@gamefield, @x, @y, @color)
		@speed = @gamefield.options.shoot.speed
		@setMass(@gamefield.options.shoot.mass)

	setTarget: (target) ->
		deg = Math.atan2(target.y, target.x)
		vel =
			x: @speed * Math.cos(deg)
			y: @speed * Math.sin(deg)
		@setBoost(vel, @gamefield.options.shoot.acceleration)

	setMass: (mass) ->
		speed = @speed
		super(mass)
		@speed = speed

	get: ->
		extend(super(), {
			type: "shoot"
		})