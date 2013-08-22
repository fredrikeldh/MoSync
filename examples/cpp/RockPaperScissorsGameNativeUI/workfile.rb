#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LSTFILES = ['Resources/res.lst']
	@LIBRARIES = ['mautil']
	@NAME = 'RockPaperScissorsGameNativeUI'
end

Works.run
