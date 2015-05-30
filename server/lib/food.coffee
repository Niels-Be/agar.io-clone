class Food extends StaticElement

	constructor: (@gamefield, @x, @y) ->
		@color = @gamefield.options.food.color
		@mass = @gamefield.options.food.mass
		@size = @gamefield.options.food.size


