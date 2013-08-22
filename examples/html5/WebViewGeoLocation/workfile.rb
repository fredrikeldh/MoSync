#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LSTFILES = ['Resources/Resources.lst']
	@LIBRARIES = ['mautil']
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'WebViewGeoLocation'
end

Works.run
