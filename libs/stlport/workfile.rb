#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ['.']
	@EXTRA_INCLUDES = ['.']

	@EXTRA_CFLAGS = ' -DUSE_EXOTIC_MATH -Wno-float-equal -Wno-unreachable-code -Wno-sign-compare -Wno-old-style-definition'
	if(@CONFIG=='release')
		# buggy compiler, buggy libs... I won't fix them.
		@EXTRA_CFLAGS += ' -Wno-uninitialized'
	end
	if(@CONFIG=='debug')
		@EXTRA_CFLAGS += ' -DMOSYNCDEBUG'
	end

	@HEADER_DIRS = [
		'.',
		'stl',
		'stl/config',
		'stl/debug',
		'stl/pointers',
		'using',
		'using/h',
	]
	@HEADER_FILE_PATTERNS = ['*']
	@HEADER_INSTALLDIR = 'stlport'

	@NAME = 'stlport'
end

Works.run
