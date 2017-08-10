
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "1.3"

# Project Metadata

define_project "tagged-format" do |project|
	project.title = "Tagged Format"
	project.summary = 'Implements the reactor pattern using fibers.'
	
	project.license = "MIT License"
	
	project.add_author 'Samuel Williams', email: 'samuel.williams@oriontransfer.co.nz'
	
	project.version = "1.0.0"
end

define_target "tagged-format-library" do |target|
	target.build do
		source_root = target.package.path + 'source'
		
		copy headers: source_root.glob('TaggedFormat/**/*.{h,hpp}')
		
		build static_library: "TaggedFormat", source_files: source_root.glob('TaggedFormat/**/*.cpp')
	end
	
	target.depends :platform
	target.depends "Build/Files"
	target.depends "Build/Clang"
	target.depends "Language/C++11", private: true
	
	target.depends "Library/Buffers"
	
	target.provides "Library/TaggedFormat" do
		append linkflags [
			->{install_prefix + 'lib/libTaggedFormat.a'},
		]
	end
end

define_target "tagged-format-executable" do |target|
	target.build do
		source_root = target.package.path + 'source'
		
		build executable: "TaggedFormat", source_files: source_root.glob('TaggedFormat.cpp')
	end
	
	target.depends :platform
	target.depends "Build/Files"
	target.depends "Build/Clang"
	target.depends "Language/C++11", private: true
	
	target.depends "Library/TaggedFormat"
	
	target.provides "Executable/TaggedFormat" do
		define Rule, "convert.tagged-format-file" do
			input :source_file, pattern: /\.tft/
			output :destination_path
			
			input :tagged_format_binary, implicit: true do |arguments|
				environment[:install_prefix] + 'bin/TaggedFormat'
			end
			
			apply do |arguments|
				mkpath File.dirname(arguments[:destination_path])
				
				root = arguments[:source_file].root
				
				run!(
					arguments[:tagged_format_binary],
					"--text-to-binary",
					arguments[:source_file].shortest_path(root),
					arguments[:destination_path].shortest_path(root),
					chdir: root
				)
			end
		end
		
		define Rule, "convert.tagged-format-files" do
			# The input prefix where files are copied from:
			input :source, multiple: true, pattern: /\.tft/
			
			# The output files:
			parameter :root
			parameter :extension, default: '.tfb'
			parameter :basename, default: true
				
			apply do |arguments|
				output_mapping = arguments.select{|key| [:root, :extension, :basename].include? key}
				
				arguments[:source].each do |path|
					destination_path = path.with(output_mapping)
					
					convert source_file: path, destination_path: destination_path
				end
			end
		end
	end
end

define_target "tagged-format-tests" do |target|
	target.build do |*arguments|
		test_root = target.package.path + 'test'
		
		run tests: "TaggedFormat", source_files: test_root.glob('TaggedFormat/**/*.cpp'), arguments: arguments
	end
	
	target.depends "Build/Clang"
	
	target.depends :platform
	target.depends "Language/C++11", private: true
	target.depends "Library/UnitTest"
	target.depends "Library/TaggedFormat"
	
	target.provides "Test/TaggedFormat"
end

# Configurations

define_configuration "development" do |configuration|
	configuration[:source] = "http://github.com/kurocha/"
	configuration.import "tagged-format"
		
	# Provides all the build related infrastructure:
	configuration.require "platforms"
	configuration.require "build-files"
	
	# Provides unit testing infrastructure and generators:
	configuration.require "unit-test"
	
	# Provides some useful C++ generators:
	configuration.require "generate-travis"
	configuration.require "generate-project"
	configuration.require "generate-cpp-class"
end

define_configuration "tagged-format" do |configuration|
	configuration.require "euclid"
	configuration.require "buffers"
end
