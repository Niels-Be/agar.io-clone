class Grid

	constructor: (@game, @options) ->


	render: (graph) ->
		#draw grid
		graph.beginPath()
		graph.lineWidth = 1
		graph.strokeStyle = @options.lineColor

		for x in [0 .. @game.gamefield.width] by @options.size
			graph.moveTo(x, 0)
			graph.lineTo(x, @game.gamefield.height)
		for y in [0 .. @game.gamefield.height] by @options.size
			graph.moveTo(0, y)
			graph.lineTo(@game.gamefield.width, y)
		
		graph.stroke()

		#draw border lines
		graph.beginPath()
		graph.lineWidth = @options.borderSize
		graph.strokeStyle = @options.borderColor

		graph.moveTo(0, 0)
		graph.lineTo(0, @game.gamefield.height)
		graph.lineTo(@game.gamefield.width, @game.gamefield.height)
		graph.lineTo(@game.gamefield.width, 0)
		graph.lineTo(0, 0)

		graph.stroke()

	# Position relative code not needed since we always draw the entiere gamefield
	###	px = @game.player.x % @options.size
		py = @game.player.y % @options.size

		graph.beginPath()
		graph.lineWidth = 1
		graph.strokeStyle = @options.lineColor
		for(x = 0 x <= view.width x += @options.size) {
			graph.moveTo(0 + x - px, 0)
			graph.lineTo(0 + x - px, view.height)
		}
		for(y = 0 y <= view.height y += @options.size) {
			graph.moveTo(0, 0 + y - py)
			graph.lineTo(view.width, 0 + y - py)
		}
		graph.stroke()

		graph.beginPath()
		graph.lineWidth = @options.borderSize
		graph.strokeStyle = @options.borderColor
		graph.moveTo(view.width/2 - @game.player.x, 0)
		graph.lineTo(view.width/2 - @game.player.x, view.height)

		graph.moveTo(view.width/2 - @game.player.x + gamefieldSize, 0)
		graph.lineTo(view.width/2 - @game.player.x + gamefieldSize, view.height)

		graph.moveTo(0, view.height/2 - @game.player.y + gamefieldSize)
		graph.lineTo(view.width, view.height/2 - @game.player.y + gamefieldSize)

		graph.moveTo(0, view.height/2 - @game.player.y)
		graph.lineTo(view.width, view.height/2 - @game.player.y)

		graph.stroke()
	###