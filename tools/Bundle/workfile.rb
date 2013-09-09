#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

NativeMoSyncExe.new do
	@SOURCES = ["."]
	@LOCAL_LIBS = ["filelist"]
	if ( HOST == :darwin )
		# Objective-C++ compiler
		@EXTRA_CPPFLAGS = " -Wno-shadow -Wno-missing-prototypes"
	end
	@NAME = "Bundle"
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
