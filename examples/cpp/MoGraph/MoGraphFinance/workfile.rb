#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

raise unless(HAVE_LIBC)

work = PipeExeWork.new
work.instance_eval do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = []
	@EXTRA_CPPFLAGS = ' -Wno-float-equal -Wno-unreachable-code -Wno-shadow -Wno-missing-noreturn -Wno-vla'
	@LIBRARIES = ["mautil", "MoGraph", "yajl"]
	use_stlport
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = "MoGraphFinance"
end

work.invoke
