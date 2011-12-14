fs = require 'fs'
{parser} = require './parser'
interfacegen = require './interfacegen'
implementationgen = require './implementationgen'

getFilename = (fullPath, callback) ->
	[path..., fileNameWithExtension] = fullPath.split '/'
	[fileNameArray..., extension] = fileNameWithExtension.split '.'
	callback fileNameArray.join ''

clarityFile = process.argv[2]
outPath = process.argv[3]

fs.readFile clarityFile, "ASCII", (err, data) ->
	model = parser.parse data
	getFilename clarityFile, (name) ->
		interfacegen.generateInterface name, model, (data) ->
			fs.writeFile "#{outPath}/#{name}.h", data, (err) ->
				if err then console.log err
		implementationgen.generateImplementation name, model, (data) ->
			fs.writeFile "#{outPath}/#{name}.c", data, (err) ->
				if err then console.log err
