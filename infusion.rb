
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

required_version "0.1"

define_package "tagged-format-0.2.0" do |package|
	package.build(:all) do |platform, environment|
		environment.use in: package.source_path do |config|
			build_path = "build-#{platform.name}-#{config.variant}"
				
			Commands.run("rm", "-rf", build_path)
			Commands.run("mkdir", "-p", build_path)
				
			Dir.chdir(build_path) do
				cmake_options = []
				cmake_options << "-DCMAKE_INSTALL_PREFIX:PATH=#{platform.prefix}"
				cmake_options << "-DCMAKE_PREFIX_PATH=#{platform.prefix}"
				cmake_options << ".."
					
				Commands.run("cmake", "-G", "Unix Makefiles", *cmake_options)
					
				Commands.run("make")
				Commands.run("make", "install")
			end
		end
	end
end
