#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

MoSyncExe.new do
	@SOURCES = ['.', 'pugixml-1.0/src']
	@SPECIFIC_CFLAGS = {
		'pugixml.cpp' => ' -Wno-shadow -Wno-float-equal -Wno-missing-noreturn'
	}
	@LOCAL_LIBS = ['filelist']
	@NAME = 'winphone-builder'
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
