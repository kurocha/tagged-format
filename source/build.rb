
add_library 'TaggedFormat' do
	def sources
		Pathname.glob(root + 'TaggedFormat/**/*.cpp')
	end
	
	def headers
		Pathname.glob(root + 'TaggedFormat/**/*.h')
	end
end

add_executable("tf-convert") do
	configure do
		linkflags ["-lTaggedFormat"]
	end
	
	def sources
		Pathname.glob(root + "tf-convert/**/*.cpp")
	end
end
