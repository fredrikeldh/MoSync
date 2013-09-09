#!/usr/bin/ruby

require File.expand_path('../../rules/cLib.rb')

LibWork.new do
	if(HOST == :win32)
		@SOURCE_FILES = ["filelist-win32.c"]
	elsif(HOST == :linux)
		@SOURCE_FILES = ["filelist-linux.c"]
		if(@CONFIG == 'release')
			# bug in /usr/include/bits/stdlib.h
			@EXTRA_CFLAGS = " -Wno-unreachable-code"
		end
	elsif(HOST == :darwin)
		@SOURCE_FILES = ["filelist-linux.c"]
	else
		error "Unsupported platform"
	end
	@SOURCE_FILES << 'copyfiles.cpp'
	@EXTRA_INCLUDES = [".."]
	@NAME = "filelist"
end

Works.run
