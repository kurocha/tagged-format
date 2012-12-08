
compile_library 'TaggedFormat' do
	def source_files(environment)
		FileList[root, 'TaggedFormat/**/*.cpp']
	end
end

copy_headers do
	def source_files(environment)
		FileList[root, 'TaggedFormat/**/*.h']
	end
end

compile_executable("tagged-format-convert") do
	configure do
		linkflags ["-lTaggedFormat"]
	end
	
	def source_files(environment)
		FileList[root, "TaggedFormat-Converter/**/*.cpp"]
	end
end
