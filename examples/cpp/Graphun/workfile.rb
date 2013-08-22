#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

raise unless(HAVE_LIBC)

MoSyncExe.new do
	@SOURCES = ['.', 'UIFramework']
	@EXTRA_CPPFLAGS = ' -Wno-shadow -Wno-float-equal'
	@LSTFILES = ['res/res.lst']
	@LIBRARIES = ['mautil']
	@PACK_PARAMETERS = " --icon #{File.expand_path('graphun.icon')}"
	@NAME = 'Graphun'
end

Works.run
