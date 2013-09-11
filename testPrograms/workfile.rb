#!/usr/bin/ruby

raise "usage: workfile <name> [options]" unless(ARGV[0])

# fetch name of program to build.
name = ARGV[0]
# normalize ARGV, so the Work can parse it.
ARGV.delete_at(0)

require File.expand_path('../rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = [name]
	@NAME = name
	@EXTRA_INCLUDES = ['.']
	@LIBRARIES = ['mautil', 'mtxml', 'mafs', 'nativeui']
	@EXTRA_LINKFLAGS = ' -heapsize 386 -stacksize 64'
	@PACK_PARAMETERS = ' --s60v3uid E1234512 --debug --permissions "Internet Access,Bluetooth"'
end

Works.run
