#!/usr/bin/ruby

require File.expand_path('../../rules/cLib.rb')

LibWork.new do
	@SOURCES = ["."]

	@EXTRA_CFLAGS = " -DHAVE_STDLIB_H -DHAVE_STRING_H";

	@SPECIFIC_CFLAGS = {
		"cp-demangle.c" => " -Wno-shadow -Wno-unreachable-code -Wno-inline"
	}

	@NAME = "demangle"
end

Works.run
