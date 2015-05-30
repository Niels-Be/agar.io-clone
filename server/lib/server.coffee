
path = require('path');
express = require('express');
app = express();
http = require('http').Server(app);
io = require('socket.io')(http);

app.use(express.static('client'));

rooms = { }

rooms["default"] = new Gamefield("default")
rooms["small"] = new Gamefield("small", {width: 2000, height: 2000})

io.on "connection", (socket) =>
	console.log("Got new Connection")

	socket.on "getRooms", =>
		socket.emit "availableRooms", (rooms[r].get() for r of rooms)

	socket.on "error", (err) =>
		console.log("Error:", err)


serveraddress = process.env.OPENSHIFT_NODEJS_IP || process.env.IP || '127.0.0.1';
serverport = process.env.OPENSHIFT_NODEJS_PORT || process.env.PORT || 3000;
http.listen serverport, serveraddress, =>
    console.log('Server listening on ' + serveraddress + ':' + serverport)
