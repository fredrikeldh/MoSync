#!/usr/bin/ruby

require File.expand_path('../../rules/cLib.rb')

LibWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["..","../../tools/ReleasePackageBuild/build_package_tools/include/"]
	@NAME = "mustache"
end

Works.run
