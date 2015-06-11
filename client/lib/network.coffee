class Network

	@Packets:
		Join: 0x10
		0x10: Packet.bind(undefined, 0x10)
		Leave: 0x11
		0x11: Packet.bind(undefined, 0x11)
		Start: 0x12
		0x12: StartPacket.bind(undefined)
		GetLobby: 0x13
		0x13: JsonPacket.bind(undefined, 0x13)

		UpdateTarget: 0x20
		0x20: TargetPacket
		SplitUp: 0x21
		0x21: Packet.bind(undefined, 0x21)
		Shoot: 0x22
		0x22: Packet.bind(undefined, 0x22)
		RIP: 0x23
		0x23: Packet.bind(undefined, 0x23)
		PlayerUpdate: 0x24
		0x24: PlayerUpdatePacket

		SetElements: 0x30
		0x30: SetElementsPacket
		UpdateElements: 0x31
		0x31: UpdateElementsPacket

		GetStats: 0xF0
		0xF0: StatsPacket

	constructor: (uri) ->
		@callbacks = {}

		@ws = new WebSocket(uri)
		@ws.binaryType = "arraybuffer"
		@ws.onopen = =>
			if @onConnect
				@onConnect()

		@ws.onclose = =>
			if @onDisconnect
				@onDisconnect()

		@ws.onerror = (err) =>
			console.err("Error:", err)

		@ws.onmessage = (e) =>
			packet = @decode(e.data)
			@callbacks[packet.id](packet)

	onConnect: (@onConnect) ->
	onDisconnect: (@onDisconnect) ->

	on: (id, func) ->
		@callbacks[id] = func

	emit: (packet) ->
		if typeof packet == "number"
			packet = new Network.Packets[packet]()
		@ws.send(packet.getData())

	decode: (data) ->
		dv = new DataView(data)
		try
			pack = new Network.Packets[dv.getUint8(0)]()
			pack.parseData(dv)
			pack
		catch err
			console.error("Packet decode error", dv.getUint8(0), err)
			


	@parseString: (dv, pos) ->
		str = []
		while true
			c = dv.getUint8(pos)
			pos++
			if c == 0
				break
			str.push c 
		[pos, uintToString(str)]

	@parseElementData: (dv, pos) ->
		res = {}
		res.id = dv.getUint32(pos, true)
		pos += 4

		res.type = dv.getUint8(pos)
		pos += 1

		[pos, res.color] = Network.parseString(dv, pos)
		[pos, res.name] = Network.parseString(dv, pos)

		res.x = dv.getFloat64(pos, true)
		pos += 8
		res.y = dv.getFloat64(pos, true)
		pos += 8
		res.size = dv.getFloat64(pos, true)
		pos += 8

		[pos, res]

	@parseElementUpdateData: (dv, pos) ->
		res = {}
		res.id = dv.getUint32(pos, true)
		pos += 4
		res.x = dv.getFloat64(pos, true)
		pos += 8
		res.y = dv.getFloat64(pos, true)
		pos += 8
		res.size = dv.getFloat64(pos, true)
		pos += 8
		res.velX = dv.getFloat64(pos, true)
		pos += 8
		res.velY = dv.getFloat64(pos, true)
		pos += 8
		[pos, res]

