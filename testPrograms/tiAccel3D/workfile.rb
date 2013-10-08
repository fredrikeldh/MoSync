#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@LIBRARIES = ["mautil"]
	@NAME = "tiAccel3D"
	@PACK_PARAMETERS = ' --debug --permissions "Internet Access,Bluetooth"'
end

Works.run
