extend = exports.extend = (object, properties) ->
	for key, val of properties
		object[key] = val
	object

sign = (x) ->
	if x 
		if x < 0 then -1 else 1
	else
		0