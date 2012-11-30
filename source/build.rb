
add_library 'TaggedFormat' do
	def sources
		FileList[root, 'TaggedFormat/**/*.cpp']
	end
	
	def headers
		FileList[root, 'TaggedFormat/**/*.h']
	end
end

add_executable("tf-convert") do
	configure do
		linkflags ["-lTaggedFormat"]
	end
	
	def sources
		FileList[root, "tf-convert/**/*.cpp"]
	end
end
