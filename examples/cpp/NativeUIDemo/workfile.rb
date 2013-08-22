#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LSTFILES = ['Resources/Resources.lst']
	@LIBRARIES = ['mautil', 'maui', 'nativeui', 'ads']
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'NativeUIDemo'
end

Works.run
