#!/usr/bin/ruby

require File.expand_path('../../rules/cExe.rb')

ExeWork.new do
	@SOURCES = ["."]
	@EXTRA_CFLAGS = " -Wno-strict-prototypes -Wno-missing-prototypes -Wno-old-style-definition -Wno-missing-noreturn -Wno-unreachable-code"
	@NAME = "protobuild"
	@COMMON_EXE = true
end

Works.run
