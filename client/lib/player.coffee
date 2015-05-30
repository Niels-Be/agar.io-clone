class Player

	constructor: (@socket, @name, @room) ->
		@socket.on "splitUp", @splitUp
		@socket.on "updateDirection", @setDirection		
		#TODO Chat

		@balls = []
		@balls.push @room.createBall()

	splitUp: ->
		newballs = []
		for ball in @balls
			newballs.push ball #TODO
		@balls.push newballs

	setDirection: (target) ->
		for ball in @balls
			ball.setDirection(target)

	update: (timediff) ->
		