#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@HEADER_INSTALLDIR = "MAFS"
	@NAME = "mafs"
end

Works.run
