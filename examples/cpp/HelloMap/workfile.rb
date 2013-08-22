#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@LSTFILES = ["Res/res.lst"]
	@LIBRARIES = ["mautil", "map", "maui"]
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = "HelloMap"
end

Works.run
