#!/usr/bin/ruby

require File.expand_path("../../rules/cLib.rb")

LibWork.new do
	@SOURCES = ['.']
	@NAME = 'sqlite'
	@EXTRA_CFLAGS = ' -D_LIB -Wno-float-equal -Wno-error -w -Wno-c++-compat'
end

Works.run
