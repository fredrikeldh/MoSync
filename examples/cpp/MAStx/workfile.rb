#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@EXTRA_CFLAGS = " -Wno-unreachable-code"
	if(USE_NEWLIB)
		@EXTRA_LINKFLAGS = standardMemorySettings(8)
	end
	@NAME = "MAStx"
end

Works.run
