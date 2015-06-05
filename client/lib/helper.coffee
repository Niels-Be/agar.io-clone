extend = (object, properties) ->
	for key, val of properties
		object[key] = val
	object


stringToUint = (string) ->
	string = unescape(encodeURIComponent(string))
	charList = string.split('')
	uintArray = []
	for c in charList
		uintArray.push(c.charCodeAt(0))
	uintArray


uintToString = (uintArray) ->
	encodedString = String.fromCharCode.apply(null, uintArray)
	decodeURIComponent(escape(encodedString))
