#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

class TypeGenTask < FileTask
	def initialize
		@prerequisites = [FileTask.new("typeGen.rb"), FileTask.new("types.rb")]
		super("stabs_builtins_generated.h")
	end
	def fileExecute
		sh "ruby typeGen.rb"
	end
end

NativeMoSyncLib.new do
	@SOURCES = ["."]
	@EXTRA_SOURCEFILES = ["../../runtimes/cpp/core/sld.cpp"]
	@EXTRA_INCLUDES = ["../../runtimes/cpp/core"]
	@EXTRA_CPPFLAGS = " -fno-exceptions -Wno-unreachable-code"
	if(!@GCC_IS_V4)	#buggy compiler
		@SPECIFIC_CFLAGS = {}
		if(CONFIG == "")
			@SPECIFIC_CFLAGS["stabs_types.cpp"] = " -Wno-uninitialized"
		end
	end
	@REQUIREMENTS = [TypeGenTask.new]
	@NAME = "stabs"
end

Works.run
