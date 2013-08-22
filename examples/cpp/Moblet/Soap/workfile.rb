#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@LIBRARIES = ["mautil", "mtxml"]
	@EXTRA_LINKFLAGS = standardMemorySettings(8) unless(USE_NEWLIB)
	@NAME = "soap"
end

Works.run
