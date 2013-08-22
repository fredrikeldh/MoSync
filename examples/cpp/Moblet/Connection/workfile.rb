#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@LIBRARIES = ["mautil"]
	@NAME = "Connection"
	@PACK_PARAMETERS = ' --s60v3uid E3450F2B --s60v2uid 00297B7A'
end

Works.run
