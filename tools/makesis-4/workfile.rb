#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')
require File.expand_path('../../rules/mosync_util.rb')

class SisWork < NativeMoSyncExe
	def init
		@SOURCES = ["src"]
		@EXTRA_INCLUDES = ["include", "src"]
		@IGNORED_FILES = ["makekeys.cpp", "dumptree.cpp", "dumpcontroller.cpp", "finddatetime.cpp"]
		@EXTRA_CPPFLAGS = " -Wno-shadow -Wno-unreachable-code"
		if(!@GCC_IS_V4 && CONFIG == "")	#buggy compiler, I think.
			@SPECIFIC_CFLAGS = {"crc.c" => " -Wno-unreachable-code"}
		end
		@LIBRARIES = ["z"]
		@CUSTOM_LIBS = ["libeay32.lib"]
		@INSTALLDIR = mosyncdir + '/bin'
	end
end

SisWork.new do
	init
	@IGNORED_FILES += ["signsis.cpp"]
	@NAME = "makesis-4"
end

SisWork.new do
	init
	@IGNORED_FILES += ["makesis.cpp"]
	@NAME = "signsis-4"
end

Works.run
