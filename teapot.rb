
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "0.8.0"

define_target "tagged-format" do |target|
	target.build do |environment|
		build_directory(package.path, 'source', environment)
	end
	
	target.depends :platform
	target.depends "Language/C++11"
	
	target.provides "Library/TaggedFormat" do
		append linkflags "-lTaggedFormat"
	end
end

define_target "tagged-format-tests" do |target|
	target.build do |environment|
		build_directory(package.path, 'test', environment)
	end
	
	target.run do |environment|
		environment = environment.flatten
		
		Commands.run(environment[:install_prefix] + "bin/tagged-format-test-runner")
	end
	
	target.depends :platform
	target.depends "Language/C++11"
	target.depends "Library/UnitTest"
	target.depends "Library/TaggedFormat"
	
	target.provides "Test/TaggedFormat"
end

define_configuration "travis" do |configuration|
	configuration[:source] = "https://github.com/dream-framework"
	
	configuration.import! "platforms"
	
	configuration.require "unit-test"
	configuration.require "euclid"
	
	configuration[:run] = ["Test/TaggedFormat"]
end

define_configuration "local" do |configuration|
	configuration[:source] = "../"
	configuration.import! "local"
end
