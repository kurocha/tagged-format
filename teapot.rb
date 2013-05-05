
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

required_version "0.7.0"

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
	
	target.depends :platform
	target.depends "Language/C++11"
	target.depends "Library/UnitTest"
	target.depends "Library/TaggedFormat"
	
	target.provides "Test/TaggedFormat"
end

define_configuration "tagged-format" do |configuration|
	configuration[:source] = "https://github.com/dream-framework"
	
	configuration.import! "platforms"
	
	# Direct dependencies:
	configuration.require "unit-test"
	configuration.require "euclid"
end

define_configuration "travis" do |configuration|
	configuration[:source] = "https://github.com/dream-framework"
	
	configuration.require "platform-linux"
	configuration.require "variants"
	
	configuration.require "unit-test"
	configuration.require "euclid"
end

define_configuration "local" do |configuration|
	configuration[:source] = "../"
	configuration.import! "local"
end
