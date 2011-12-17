fs = require 'fs'
{exec} = require 'child_process'

srcDir = 'src'
genDir = 'gen'

libSrcDir = "#{srcDir}/lib"
testSrcDir = "../#{srcDir}/test"
libGenDir = "#{genDir}/lib"
testGenDir = "test"
libGenCovDir = "lib-cov"
testReportPath = "report.txt"
itSrcDir = "../#{srcDir}/itdata"
itGenDir = "../#{genDir}/itdata"

build = (claPath, src, dst, callback) ->
	fs.readdir src, (err, files) ->
		count = 0
		unless files? then callback() if callback?
		else files.map (file, index) ->
			if fs.statSync("#{src}/#{file}").isDirectory()
				count++
				build "#{src}/#{file}", "#{dst}/#{file}", ->
					count--
					callback() if count is 0 and callback?
			else
				extension = file.substr ((file.lastIndexOf '.') + 1)
				if extension is "coffee"
					count++
					exec "coffee -l --bare --output #{dst} --compile
					 #{src}/#{file}", (err, stdout, stderr) ->
						console.log stderr if stderr
						console.log stdout if stdout
						console.log err if err
						count--
						callback() if count is 0 and callback?
				else if extension is "c"
					count++
					[fileNameArray..., extension] = file.split '.'
					fileName = fileNameArray.join ''
					exec "./checkpatch.pl --no-typedef --line=80 --tab=4 --no-tree -q -f #{src}/#{file}", (err, stdout, stderr) ->
						console.log stdout if stdout
						exec "mkdir -p #{dst};clang -pedantic -ansi -c #{src}/#{file} -I../lib -o #{dst}/#{fileName}.o", (err, stdout, stderr) ->
							console.log stderr if stderr
							console.log stdout if stdout
							console.log err if err
							count--
							callback() if count is 0 and callback?
				else if extension is "js" or extension is "h"
					count++
					exec "./checkpatch.pl --no-typedef --line=80 --tab=4 --no-tree -q -f #{src}/#{file}", (err, stdout, stderr) ->
						console.log stdout if stdout
						exec "mkdir -p #{dst};cp #{src}/#{file} #{dst}/#{file}", (err, stdout, stderr) ->
							console.log stderr if stderr
							console.log stdout if stdout
							console.log err if err
							count--
							callback() if count is 0 and callback?
				else if extension is "yy"
					count++
					[fileNameArray..., extension] = file.split '.'
					fileName = fileNameArray.join ''
					exec "mkdir -p #{dst};jison #{src}/#{file} -o #{dst}/#{fileName}.js", (err, stdout, stderr) ->
						console.log stderr if stderr
						console.log stdout if stdout
						console.log err if err
						count--
						callback() if count is 0 and callback?
				else if extension is "cla"
					count++
					[fileNameArray..., extension] = file.split '.'
					fileName = fileNameArray.join ''
					exec "node ../#{claPath}/clarity.js #{src}/#{file} #{dst}", (err, stdout, stderr) ->
						console.log stderr if stderr
						console.log stdout if stdout
						console.log err if err
						count--
						callback() if count is 0 and callback?

pretest = (callback) ->
	process.chdir genDir
	build libGenDir, testSrcDir, testGenDir, ->
		fs.readdir testGenDir, (err, files) ->
			unless files? then callback()
			else 
				paths = files.map (file) -> "#{testGenDir}/#{file}"
				pathsString = paths.join ' '
				callback pathsString if callback?

posttest = (err, stdout, stderr, callback) ->
	console.log stderr if stderr
	console.log stdout if stdout
	console.log err if err
	fs.writeFile testReportPath, stdout, (err) ->
		console.log err if err
	callback() if callback?
    
test = (callback) ->
	pretest (pathsString) ->
		unless pathsString?
			callback() if callback?
		else
			exec "NODE_PATH=lib-cov/ whiskey --coverage -t \"#{pathsString}\"",
			(err, stdout, stderr) ->
				posttest err, stdout, stderr, ->
					callback() if callback?

htmltest = (callback) ->
	pretest (pathsString, callback) ->
		exec "NODE_PATH=lib-cov/ whiskey --coverage
		--coverage-reporter html --coverage-dir cov
		-t \"#{pathsString}\"",
		(err, stdout, stderr) ->
			posttest err, stdout, stderr, ->
				callback() if callback?

buildAll = (callback) ->
	build libGenDir, libSrcDir, libGenDir, callback

integrationTestAll = (callback) ->
	build libGenDir, itSrcDir, itGenDir, callback

task 'all', 'build, test all and run integration tests', ->
	buildAll ->
		test ->
			integrationTestAll()
		
task 'bt', 'build and test all', ->
	buildAll ->
		test()

task 'bth', 'build and run all tests with html coverage report', ->
	buildAll ->
		htmltest()

task 'build', 'build all files', ->
	buildAll()

task 'test', 'run all tests', ->
	test()

task 'test-html', 'run all tests with html coverage report', ->
	htmltest()

