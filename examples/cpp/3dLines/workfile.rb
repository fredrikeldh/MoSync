#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = " -Wno-shadow"
	@NAME = "3dLines"
end

Works.run
