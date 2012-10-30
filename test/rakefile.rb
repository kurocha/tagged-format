
task :test do
	sh("rm -rf build")
	sh("mkdir build")
	
	Dir.chdir("build") do
		unless ENV['CXX']
			ENV['CXX'] = "clang++"
			ENV['CXXFLAGS'] = "-std=c++11 -stdlib=libc++ -g -Wall"
		else
			ENV['CXXFLAGS'] = "-std=c++11 -g -Wall"
		end
		
		sh("cmake ../")
		sh("make")
		
		sh("./test-runner --on-failure ./dump-backtrace")
	end
end

task :default => :test
