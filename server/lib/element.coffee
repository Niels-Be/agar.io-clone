class Element

	constructor: (@gamefield, @x, @y, @color, @size = 1) ->
		@id = -1
		@sizechanged = false

	intercept: (other) ->
		distance = (@x - other.x) * (@x - other.x) + (@y - other.y) * (@y - other.y)
		distance <= (@size*@size)

	canEat: (other) ->
		false

	get: ->
		{
			id: @id
			x: @x
			y: @y
			color: @color
			size: @size
		}

	getPos: ->
		res =
			id: @id
			x: @x
			y: @y
		if @sizechanged #save some bytes
			res.size = @size 
		res

class StaticElement extends Element

	update: (t) ->


class MoveableElement extends Element

	constructor: (@gamefield, @x, @y, @color, @size = 1, @speed = 100) ->
		super(@gamefield, @x, @y, @color, @size)
		
		@velocity =
			x: 0
			y: 0

		@boostVel =
			x: 0
			y: 0

		@boostAcc = 0

		@target =
			x: 0
			y: 0

	setBoost: (@boostVel, @boostAcc) ->

	setTarget: (@target) ->

	update: (t) ->

		if @boostVel.x != 0 || @boostVel.y != 0
			ba = -@boostAcc

			velX = ba * t * sign(@boostVel.x) + @boostVel.x
			velY = ba * t * sign(@boostVel.y) + @boostVel.y

			@boostVel.x = if sign(velX) == sign(@boostVel.x) then velX else 0
			@boostVel.y = if sign(velY) == sign(@boostVel.y) then velY else 0


		#a = @gamefield.options.player.force / (@mass + 1)
		a = @gamefield.options.player.acceleration #constant acceleration independet of mass

		#switch ifs to allow still standing
		#if @target.x*@target.x + @target.y*@target.y > @size*@size/4
		if @target.x != 0 || @target.y != 0 #prevent devision by 0
			deg = Math.atan2(@target.y, @target.x)
			degX = Math.cos(deg)
			degY = Math.sin(deg)

			velX = a * t * degX + @velocity.x
			velY = a * t * degY + @velocity.y

			@velocity.x = if Math.abs(velX) > Math.abs(@speed*degX) then @speed*degX else velX
			@velocity.y = if Math.abs(velY) > Math.abs(@speed*degY) then @speed*degY else velY
		else
			velX = @velocity.x - a * t * sign(@velocity.x)
			velY = @velocity.y - a * t * sign(@velocity.y)

			@velocity.x = if sign(velX) == sign(@velocity.x) then velX else 0
			@velocity.y = if sign(velY) == sign(@velocity.y) then velY else 0

		deltaX = (@boostVel.x + @velocity.x) * t
		deltaY = (@boostVel.y + @velocity.y) * t

		#do not let them move outside the gamefield
		if(0 <= @x + deltaX <= @gamefield.options.width)
			@x += deltaX
		if(0 <= @y + deltaY <= @gamefield.options.height)
			@y += deltaY

		#if they got outside somehow, let them move back in
		if(@x < 0 && deltaX > 0 || @x > @gamefield.options.width && deltaX < 0)
			@x += deltaX
		if(@y < 0 && deltaY > 0 || @y > @gamefield.options.height && deltaY < 0)
			@y += deltaY

	get: ->
		vel = 
			x: (@boostVel.x + @velocity.x)
			y: (@boostVel.y + @velocity.y)
		extend(super(), {
			velocity: vel
		})

	getPos: ->
		vel = 
			x: (@boostVel.x + @velocity.x)
			y: (@boostVel.y + @velocity.y)
		extend(super(), {
			velocity: vel
		})
