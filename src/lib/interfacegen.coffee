stringgen = require './stringgen'

handleEmpty = (data, callback) ->
	callback ""

handleStructTable = (data, callback) ->
	array = ""
	structArray = data.body.map (item) ->
		if item.length? then array = "[#{item.length}]"
		"\t#{item.type}* #{item.name}#{array};"
	callback "\ntypedef struct {\n#{structArray.join '\n'}\n} #{data.name};\n"

handlePrototypeConstTable = (data, callback) ->
	callback "\n#{data.body.type} const * const #{data.name};\n"

handleEnumTable = (data, callback) ->
	enumArray = data.body.map (item) ->
		value = ","
		if item.value? then value = " = #{item.value},"
		"\t#{item.name}#{value}"
	callback "\ntypedef enum {\n#{enumArray.join '\n'}\n} #{data.name};\n"

handlePrototypeExecutionTable = (data, callback) ->
	input = "void"
	if data.input?
		paramArray = data.input.map (item) ->
			"#{item.type}* const #{item.name}"
		input = paramArray.join ', '
	output = "void"
	if data.output? then output = data.output
	callback "\n#{output} #{data.name}(#{input});\n"

handleSignature = (data, callback) ->
	input = "void"
	if data.input?
		paramArray = data.input.map (item) ->
			"#{item.type}* const #{item.name}"
		input = paramArray.join ', '
	output = "void"
	if data.output? then output = data.output
	callback "\ntypedef #{output}(*#{data.name})(#{input});\n"

handleRequire = (data, callback) ->
	callback "\#include \"#{data.value}.h\"\n"

router =
	'struct': handleStructTable
	'const': handlePrototypeConstTable
	'enum': handleEnumTable
	'parallel': handlePrototypeExecutionTable
	'serial': handlePrototypeExecutionTable
	'conditional': handlePrototypeExecutionTable
	'require': handleRequire
	'signature': handleSignature

generateInterfaceBody = (data, callback) ->
	dataArray = []
	count = 0
	bodyArray = data.map (item, index) ->
		count++
		process.nextTick ->
			if item.scope is 'external'
				handler = router[item.type] ? handleEmpty
			else
				handler = handleEmpty
			handler item, (data) ->
				count--
				bodyArray[index] = data
				if count is 0
					stringgen.generateString dataArray, callback
	dataArray.push bodyArray

generateInterfaceHeader = (name, callback) ->
	callback "\#ifndef __#{name.toUpperCase()}_H__" +
	"\n#define __#{name.toUpperCase()}_H__" +
	"\n\#include \"Clarity.h\"\n"

generateInterfaceFooter = (callback) ->
	callback "\n#endif\n"

exports.generateInterface = (name, data, callback) ->
	generateInterfaceHeader name, (header) ->
		generateInterfaceBody data, (body) ->
			generateInterfaceFooter (footer) ->
				callback header + body + footer
