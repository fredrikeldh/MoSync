#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@LIBRARIES = ["mautil","nativeui"]
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = "HelloNativeUI"
end

Works.run
