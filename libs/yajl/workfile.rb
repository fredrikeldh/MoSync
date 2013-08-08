#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ['.', 'src']
	@SOURCES << 'src/stdlib' if(!HAVE_LIBC)

	@HEADER_DIRS = ['.', 'src/api']
	@EXTRA_INCLUDES = ['src/stdlib'] if(!HAVE_LIBC)
	@EXTRA_CFLAGS = ' -Wno-float-equal -Wno-unreachable-code -Wno-c++-compat'

	@HEADER_INSTALLDIR = 'yajl'
	@NAME = 'yajl'
end

Works.run
