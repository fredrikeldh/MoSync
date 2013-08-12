#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ['.']
	@HEADER_DIRS = ['.']
	@EXTRA_INCLUDES = ['..']
	use_stlport
	@EXTRA_CPPFLAGS = ' -Wno-float-equal -Wno-unreachable-code -Wno-shadow -Wno-missing-noreturn'

	@HEADER_INSTALLDIR = 'MoGraph'
	@NAME = 'MoGraph'
end

Works.run
