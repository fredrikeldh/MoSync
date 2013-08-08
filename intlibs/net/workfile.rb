#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

NativeMoSyncLib.new do
	@SOURCES = ["."]
	@NAME = "net"
end

Works.run
