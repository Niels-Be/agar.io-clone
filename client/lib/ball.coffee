class Ball
	
	constructor: (@options, data) ->
		extend @, data
		@updates = 0

	updateData: (data) ->
		extend @, data

	update: (t) ->
		@updates++
		if @velocity
			@x += @velocity.x * t
			@y += @velocity.y * t

	render: (graph) ->
		graph.beginPath();
		graph.strokeStyle = @options.borderColor;
		graph.fillStyle = @color || @options.fillColor;
		graph.lineWidth = @options.border;
		graph.arc(@x, @y, @size, 0, 2 * Math.PI);
		graph.stroke();
		graph.fill();

		if @name && @name != ""
			fontSize = Math.max(@size / 2, @options.defaultSize);
			graph.lineWidth = @options.textBorderSize;
			graph.textAlign = "center";
			graph.fillStyle = @options.textColor;
			graph.textBaseline = 'middle';
			graph.strokeStyle = @options.textBorder;
			graph.font = "bold " + fontSize + "px sans-serif";
			graph.strokeText(@name, @x, @y);
			graph.fillText(@name, @x, @y);

