#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@HEADER_INSTALLDIR = "Notification"
	@NAME = "Notification"
end

Works.run
