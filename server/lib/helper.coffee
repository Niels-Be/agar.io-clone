###
extend = exports.extend = (object, properties) ->
	for key, val of properties
		object[key] = val
	object
###
extend = exports.extend = (destination, source) ->
	for key, val of source
		if Array.isArray(val)
			destination[key] = val.slice(0)
		else if typeof val == "object" && val != null
			destination[key] = destination[key] || {}
			arguments.callee(destination[key], val)
		else
			destination[key] = val
	destination

sign = (x) ->
	if x 
		if x < 0 then -1 else 1
	else
		0