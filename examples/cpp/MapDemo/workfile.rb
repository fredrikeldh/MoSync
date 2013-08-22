#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["."]
	@EXTRA_CPPFLAGS = " -Wno-shadow"
	@LSTFILES = ["Res/res.lst"]
	@LIBRARIES = ["mautil", "map", "maui"]
	@EXTRA_LINKFLAGS = ' -heapsize 386 -stacksize 64'
	@NAME = "MapDemo"
end

Works.run
