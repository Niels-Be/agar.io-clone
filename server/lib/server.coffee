
path = require('path');
express = require('express');
app = express();
http = require('http').Server(app);
io = require('socket.io')(http);

app.use(express.static('client'));

rooms = { }

rooms["default"] = new Gamefield("default")
rooms["small"] = new Gamefield("small", {
		width: 2000
		height: 2000
		obstracle:
			max: 2
	})

io.on "connection", (socket) =>
	console.log("Got new Server connection")

	socket.on "getRooms", =>
		socket.emit "availableRooms", (rooms[r].get() for r of rooms)

	socket.on "createRoom", (name, options) =>
		if not rooms.hasOwnProperty(name)
			rooms[name] = new Gamefield(name, options)
		else
			socket.emit "roomCreateFailed", "Name already exists"

	socket.on "error", (err) =>
		console.log("Error:", err)


serveraddress = process.env.OPENSHIFT_NODEJS_IP || process.env.IP || '127.0.0.1';
serverport = process.env.OPENSHIFT_NODEJS_PORT || process.env.PORT || 3000;
http.listen serverport, serveraddress, =>
    console.log('Server listening on ' + serveraddress + ':' + serverport)
