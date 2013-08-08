#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = " -Wno-shadow"	#TODO, low-prio: enable -Wshadow
	@HEADER_INSTALLDIR = "MATest"
	@NAME = "matest"
end

Works.run
