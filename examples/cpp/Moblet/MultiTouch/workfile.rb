#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LIBRARIES = ['mautil']
	@NAME = 'MultiTouch'
end

Works.run
