#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@HEADER_INSTALLDIR = "Purchase"
	@NAME = "Purchase"
end

Works.run
