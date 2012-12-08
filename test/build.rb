
compile_executable("tagged-format-test-runner") do
	def source_files(environment)
		FileList[root, "**/*.cpp"]
	end
end
