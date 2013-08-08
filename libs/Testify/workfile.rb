#!/usr/bin/ruby
require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@EXTRA_INCLUDES = ["inc"]
	@SOURCES = ["src", "inc"]
	@HEADER_INSTALLDIR = "testify"
	@NAME = "testify"
end

Works.run
