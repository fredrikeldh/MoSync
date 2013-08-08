#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')
require File.expand_path('../../rules/mosync_util.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = ""

	@NAME = "updater"

	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
