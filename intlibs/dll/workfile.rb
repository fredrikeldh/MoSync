#!/usr/bin/ruby

require File.expand_path('../../rules/cLib.rb')

LibWork.new do
	if(HOST == :win32)
		@SOURCE_FILES = ["dll-win32.cpp"]
	elsif(HOST == :linux)
		@SOURCE_FILES = ["dll-unix.cpp"]
	elsif(HOST == :darwin)
		@SOURCE_FILES = ["dll-unix.cpp"]
	else
		error "Unsupported platform"
	end
	@NAME = "dll"
end

Works.run
