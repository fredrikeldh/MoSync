#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@IGNORED_FILES = ["MemoryMgr.cpp"]
	@SPECIFIC_CFLAGS = {
		"MapCache.cpp" => " -Wno-unreachable-code",
		"LonLat.cpp" => " -Wno-float-equal",
	}
	@HEADER_INSTALLDIR = "MAP"
	@NAME = "map"
end

Works.run
