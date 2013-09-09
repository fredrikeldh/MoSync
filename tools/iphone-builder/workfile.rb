#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

NativeMoSyncExe.new do
	@SOURCES = ["."]
	@LOCAL_LIBS = ["filelist"]
	@NAME = "iphone-builder"
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
