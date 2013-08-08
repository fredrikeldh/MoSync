#!/usr/bin/ruby

require File.expand_path('../../rules/cLib.rb')

LibWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = [".."]
	if(!@GCC_IS_V4 && @CONFIG == 'release')
		# gcc's C++ optimizer has a few bugs...
		@EXTRA_CPPFLAGS = " -Wno-uninitialized"
	end
	@NAME = "idl-common"
end

Works.run
