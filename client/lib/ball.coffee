class Ball
	
	constructor: (@options, data) ->
		extend @, data

	updateData: (data) ->
		extend @, data

	update: (t) ->
		if @velX
			@x += @velX * t
		if @velY
			@y += @velY * t

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

