#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

NativeMoSyncExe.new do
	@SOURCES = ["src"]
	@EXTRA_INCLUDES = ["include"]
	@EXTRA_CPPFLAGS = " -Wno-shadow -Wno-unreachable-code"
	@LIBRARIES = ["z"]

	@NAME = "makesis-200"
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
