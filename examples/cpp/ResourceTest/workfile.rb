#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LIBRARIES = ['mautil', 'maui']
	@LSTX = 'Res/res.lstx'
	@RES_PLATFORM = 'windows phone'
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'ResourceTest'
end

Works.run
