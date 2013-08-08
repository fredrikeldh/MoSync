#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = ""
	@NAME = "mifconv"
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
