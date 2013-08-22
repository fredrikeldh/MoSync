#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LIBRARIES = ['mautil']
	@EXTRA_CPPFLAGS = ' -Wno-float-equal'
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'SensorTest'
end

Works.run
