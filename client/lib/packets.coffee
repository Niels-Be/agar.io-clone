
class Packet
	constructor: (@id) ->

	parseData: (data) ->

	getData: ->
		ar = new ArrayBuffer(1)
		dv = new DataView(ar)
		dv.setUint8(0, @id)
		ar

class JoinPacket extends Packet
	constructor: (@lobby) ->
		super(0x10)

	getData: ->
		ar = new ArrayBuffer(1+4)
		dv = new DataView(ar)
		dv.setUint8(0, @id)
		dv.setUint32(1, @lobby, true)
		ar

class StartPacket extends Packet
	constructor: (@name) ->
		super(0x12)

	getData: ->
		strbuf = stringToUint(@name)
		ar = new Uint8Array(1+strbuf.length)
		ar.set([@id], 0)
		ar.set(strbuf, 1)
		ar.buffer

class TargetPacket extends Packet
	constructor: (@x, @y) ->
		super(0x20)

	getData: ->
		ar = new ArrayBuffer(1+8+8)
		dv = new DataView(ar)
		dv.setUint8(0, @id)
		dv.setFloat64(1, @x, true)
		dv.setFloat64(9, @y, true)
		ar

class PlayerUpdatePacket extends Packet
	constructor: ->
		super(0x24)

	parseData: (data) ->
		@mass = data.getUint32(1, true)
		@balls = []
		pos = 5
		while pos < data.byteLength
			@balls.push data.getUint32(pos, true)
			pos += 4

class SetElementsPacket extends Packet
	constructor: ->
		super(0x30)

	parseData: (data) ->
		@elements = []
		pos = 1
		while pos < data.byteLength
			[pos, e] = Network.parseElementData(data, pos)
			@elements.push e
		

class UpdateElementsPacket extends Packet
	constructor: ->
		super(0x31)

	parseData: (data) ->
		@newElements = []
		@deletedElements = []
		@updateElements = []
		count = data.getUint16(1, true)
		pos = 3
		for [0...count]
			[pos, e] = Network.parseElementData(data, pos)
			@newElements.push e
		count = data.getUint16(pos, true)
		pos += 2
		for [0...count]
			@deletedElements.push data.getUint32(pos, true)
			pos += 4
		while pos < data.byteLength
			[pos, e] = Network.parseElementUpdateData(data, pos)
			@updateElements.push e

class StatsPacket extends Packet
	constructor: ->
		super(0xF0)

	parseData: (data) ->
		@update = data.getFloat64(1, true)
		@collision = data.getFloat64(1+8, true)
		@other = data.getFloat64(1+8+8, true)
		@elementCount = data.getUint32(1+8+8+8, true)
		@playerCount = data.getUint32(1+8+8+8+4, true)

class JsonPacket extends Packet
	constructor: (@id, @data) ->
		super(@id)

	getData: ->
		console.log("JsonData", @id, @data)
		if @data
			strbuf = stringToUint(JSON.stringify(@data))
			ar = new Uint8Array(1+strbuf.length)
			ar.set([@id], 0)
			ar.set(strbuf, 1)
			ar.buffer
		else
			super()


	parseData: (data) ->
		[pos, str] = Network.parseString(data, 1)
		@data = JSON.parse(str)
