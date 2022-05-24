# Teapot v3.4.1 configuration generated at 2022-05-23 20:27:04 +1200

required_version "3.0"

define_project "tagged-format" do |project|
	project.title = "Tagged Format"
	
	project.license = "MIT License"
	
	project.add_author 'Samuel Williams', email: 'samuel.williams@oriontransfer.co.nz'
	
	project.version = "1.0.0"
end

# Build Targets

define_target 'tagged-format-library' do |target|
	target.depends 'Language/C++14'
	target.depends "Library/Buffers", public: true
	
	target.provides 'Library/TaggedFormat' do
		source_root = target.package.path + 'source'
		
		library_path = build static_library: 'TaggedFormat', source_files: source_root.glob('TaggedFormat/**/*.cpp')
		
		append linkflags library_path
		append header_search_paths source_root
	end
end

define_target 'tagged-format-test' do |target|
	target.depends 'Library/TaggedFormat'
	target.depends 'Library/UnitTest'
	
	target.depends 'Language/C++14'
	
	target.provides 'Test/TaggedFormat' do |arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'TaggedFormat-tests', source_files: test_root.glob('TaggedFormat/**/*.cpp'), arguments: arguments
	end
end

define_target 'tagged-format-executable' do |target|
	target.depends 'Library/TaggedFormat'
	
	target.depends 'Language/C++14'
	
	target.provides 'Executable/TaggedFormat' do
		source_root = target.package.path + 'source'
		
		executable_path = build executable: 'TaggedFormat', source_files: source_root.glob('TaggedFormat.cpp')
		
		tagged_format_executable executable_path
		
		define Rule, "convert.tagged-format-file" do
			input :source_file, pattern: /\.tft/
			output :destination_path
			
 			apply do |arguments|
				mkpath File.dirname(arguments[:destination_path])
				
				root = arguments[:source_file].root
				
				run!(
					environment[:tagged_format_executable],
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
					destination_path = path.with(**output_mapping)
					
					convert source_file: path, destination_path: destination_path
				end
			end
		end
	end
end

define_target 'tagged-format-run' do |target|
	target.depends 'Executable/TaggedFormat'
	
	target.depends :executor
	
	target.provides 'Run/TaggedFormat' do |*arguments|
		run executable_file: environment[:tagged_format_executable], arguments: arguments
	end
end

# Configurations

define_configuration 'development' do |configuration|
	configuration[:source] = "https://github.com/kurocha"
	configuration.import "tagged-format"
	
	# Provides all the build related infrastructure:
	configuration.require 'platforms'
	
	# Provides unit testing infrastructure and generators:
	configuration.require 'unit-test'
	
	# Provides some useful C++ generators:
	configuration.require 'generate-cpp-class'
	
	configuration.require "generate-project"
end

define_configuration "tagged-format" do |configuration|
	configuration.public!
	configuration.require "euclid"
	configuration.require "buffers"
end
