#!/usr/bin/ruby

require File.expand_path('../../../../rules/cLib.rb')

LibWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["../../.."]
	@NAME = "mosync_log_file"
end

Works.run
