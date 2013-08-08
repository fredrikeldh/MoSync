#!/usr/bin/ruby

require File.expand_path('../../rules/cLib.rb')

LibWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["..","../../tools/ReleasePackageBuild/build_package_tools/include/"]
	@LOCAL_LIBS = ["filelist"]
	@NAME = "profiledb"
	if(HOST==:linux || HOST==:darwin)
		@LIBRARIES = ["expat"]
	else    # win32
		@LIBRARIES = ["libexpat.lib"]
	end
end

Works.run
