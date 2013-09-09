#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

NativeMoSyncExe.new do
	@SOURCES = ["."]
	@NAME = "extcomp"
	@TARGETDIR = "."
    if ( HOST == :darwin )
        # Objective-C++ compiler
        @EXTRA_CPPFLAGS = " -Wno-shadow -Wno-missing-prototypes"
    end
	@LOCAL_LIBS = ["idl-common", "filelist", "profiledb"]

	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
