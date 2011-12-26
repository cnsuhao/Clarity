#Copyright 2011 Patchwork Solutions AB. All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#     1. Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#
#     2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#THIS SOFTWARE IS PROVIDED BY Patchwork Solutions AB ``AS IS'' AND ANY EXPRESS
#OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
#OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
#EVENT SHALL Patchwork Solutions AB OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
#OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#The views and conclusions contained in the software and documentation are
#those of the authors and should not be interpreted as representing official
#policies, either expressed or implied, of Patchwork Solutions AB.
stringgen = require './stringgen'

handleEmpty = (data, callback) ->
	callback ""

handleStructTable = (data, callback) ->
	retVal = ""
	if data.scope is 'internal'
		structArray = data.body.map (item) ->
			array = ""
			if item.length? then array = "[#{item.length}]"
			"\t#{item.type}* #{item.name}#{array};"
		retVal = "\ntypedef struct {\n#{structArray.join '\n'}\n} #{data.name};\n"
	callback retVal

handleConstTable = (data, callback) ->
	scope = ""
	if data.scope is 'internal' then scope = "static "
	callback "\nstatic const #{data.body.type} __#{data.name} = #{data.body.value};\n" +
		"#{scope}#{data.body.type} const * const #{data.name} = &__#{data.name};\n"

handleEnumTable = (data, callback) ->
	retVal = ""
	if data.scope is 'internal'
		enumArray = data.body.map (item) ->
			value = ","
			if item.value?
				value = " = #{item.value},"
			"\t#{item.name}#{value}"
		retVal = "\ntypedef enum {\n#{enumArray.join '\n'}\n} #{data.name};\n"
	callback retVal

handlePrototypeExecutionTable = (data, callback) ->
	retVal = ""
	if data.scope is 'internal'
		input = "void"
		if data.input?
			paramArray = data.input.map (item) ->
				"#{item.type}* const #{item.name}"
			input = paramArray.join ', '
		output = "void"
		if data.output? then output = data.output
		retVal = "\nstatic #{output} #{data.name}(#{input});"
	callback retVal

handleExecutionTableHeader = (data, callback) ->
	scope = ""
	if data.scope is 'internal' then scope = "static "
	input = "void"
	if data.input?
		paramArray = data.input.map (item) ->
			"#{item.type}* const #{item.name}"
		input = paramArray.join ', '
	output = "void"
	if data.output? then output = data.output
	callback "\n#{scope}#{output} #{data.name}(#{input})\n{"

handleParallelExecutionTable = (data, callback) ->
	handleExecutionTableHeader data, (header) ->
		generateImplementation data.body, parallelExecutionRouter, (body) ->
			callback "#{header}#{body}\n}\n"

handleExecutionStatement = (data, callback) ->
	params = ""
	if data.params? then params = data.params.join ', '
	callback "\n\t#{data.name}(#{params});"

handleExecutionDeclaration = (data, callback) ->
	callback "\n\t#{data.declaration.type} __#{data.declaration.name};" +
		"\n\t#{data.declaration.type}* #{data.declaration.name} = &__#{data.declaration.name};"

handleExecutionAssignment = (data, callback) ->
	console.log 'blarf'
	callback ""

handleSerialExecutionTable = (data, callback) ->
	handleExecutionTableHeader data, (header) ->
		generateImplementation data.body, serialExecutionRouter, (body) ->
			if  data.output?
				[body..., lastLine] = body.split '\n\t'
				body.push "return " + lastLine
				body = body.join "\n\t"
			callback "#{header}#{body}\n}\n"

handleConditionalExecutionTable = (data, callback) ->
	handleExecutionTableHeader data, (header) ->
		callback "#{header}\n}\n"

handleRequire = (data, callback) ->
	retVal = ""
	if data.scope is 'internal' then retVal = "\#include \"#{data.value}.h\"\n"
	callback retVal

handleSignature = (data, callback) ->
	retVal = ""
	if data.scope is 'internal'
		output = "void"
		if data.output? then output = data.output
		paramArray = data.input.map (item) ->
			"#{item.type}* const #{item.name}"
		retVal = "\ntypedef #{output}(*#{data.name})(#{paramArray.join ', '});\n"
	callback retVal

headerRouter =
	'struct': handleStructTable
	'const': handleConstTable
	'enum': handleEnumTable
	'parallel': handlePrototypeExecutionTable
	'serial': handlePrototypeExecutionTable
	'conditional': handlePrototypeExecutionTable
	'require': handleRequire
	'signature': handleSignature

bodyRouter =
	'parallel': handleParallelExecutionTable
	'serial': handleSerialExecutionTable
	'conditional': handleConditionalExecutionTable

parallelExecutionRouter =
	'statement': handleExecutionStatement

serialExecutionRouter =
	'statement': handleExecutionStatement
	'declaration': handleExecutionDeclaration
	'assignment': handleExecutionAssignment


generateImplementation = (data, router, callback) ->
	dataArray = []
	count = 0
	bodyArray = data.map (item, index) ->
		count++
		process.nextTick ->
			handler = router[item.type] ? handleEmpty
			handler item, (data) ->
				count--
				bodyArray[index] = data
				if count is 0
					stringgen.generateString dataArray, callback
	dataArray.push bodyArray

generateImplementationHeader = (name, data, callback) ->
	generateImplementation data, headerRouter, (header) ->
		callback "\#include \"#{name}.h\"\n#{header}"

exports.generateImplementation= (name, data, callback) ->
	generateImplementationHeader name, data, (header) ->
		generateImplementation data, bodyRouter, (body) ->
			callback header + "\n" + body
