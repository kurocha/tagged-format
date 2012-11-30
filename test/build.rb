
add_executable("tagged-format-test-runner") do
	configure do
		linkflags ["-lUnitTest", "-lTaggedFormat"]
	end
	
	def sources
		FileList[root, "**/*.cpp"]
	end
end
