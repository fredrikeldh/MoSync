#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = " -Wno-shadow"	#TODO, low-prio: enable -Wshadow
	@HEADER_INSTALLDIR = "maui-revamp/MAUI"
	@IGNORED_HEADERS = ["DefaultSkin.h"]
	@IGNORED_FILES = ["test.cpp"]
	@NAME = "maui-revamp"
end

Works.run
