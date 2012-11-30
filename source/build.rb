
add_library 'TaggedFormat' do
	def sources(environment)
		FileList[root, 'TaggedFormat/**/*.cpp']
	end
	
	def headers(environment)
		FileList[root, 'TaggedFormat/**/*.h']
	end
end

add_executable("tagged-format-convert") do
	configure do
		linkflags ["-lTaggedFormat"]
	end
	
	def sources(environment)
		FileList[root, "TaggedFormat-Converter/**/*.cpp"]
	end
end
