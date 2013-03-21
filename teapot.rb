
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

required_version "0.6"

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
