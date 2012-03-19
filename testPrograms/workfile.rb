#!/usr/bin/ruby

raise "usage: workfile <name> [options]" unless(ARGV[0])

# fetch name of program to build.
name = ARGV[0]
# normalize ARGV, so the Work can parse it.
ARGV.delete_at(0)

require File.expand_path('../rules/mosync_exe.rb')

work = PipeExeWork.new
work.instance_eval do
	@SOURCES = [name]
	@EXTRA_SOURCEFILES = [name]
	@NAME = name
	@EXTRA_INCLUDES = ['.']
	@LIBRARIES = ['mautil', 'mtxml']
	#@EXTRA_LINKFLAGS = ' -datasize=1024000 -heapsize=386000 -stacksize=64000' if(!USE_ARM)
end

work.invoke
