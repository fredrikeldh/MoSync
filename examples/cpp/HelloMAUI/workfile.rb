#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@LIBRARIES = ["mautil", "maui"]
	@EXTRA_LINKFLAGS = " -heapsize 128 -stacksize 16" unless(USE_NEWLIB)
	@NAME = "HelloMAUI"
end

Works.run
