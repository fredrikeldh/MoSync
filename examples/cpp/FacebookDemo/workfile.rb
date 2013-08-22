#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.', 'GUI', 'Application']
	#@EXTRA_CPPFLAGS = " -Wno-shadow"
	@LIBRARIES = ['mautil', 'maui', 'Facebook', 'yajl', 'nativeui']
	@EXTRA_LINKFLAGS = ' -heapsize 386 -stacksize 64'
	@NAME = 'FacebookDemo'
end

Works.run
