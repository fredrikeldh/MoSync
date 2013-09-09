#!/usr/bin/ruby

require File.expand_path('../shared_work.rb')

# todo: make sure only syscalls are exported.

NativeMoSyncDll.new do
	extend SdlCommon
	setup_common

	@SOURCE_FILES = ["main.cpp"]
	@SPECIFIC_CFLAGS = {"main.cpp" => " -DMOSYNC_DLL_EXPORT -Wno-missing-noreturn",
		"mosyncmain.cpp" => " -DMOSYNC_DLL_IMPORT"}
	# mosync_sdl must be first, since it depends on the others.
	@LOCAL_LIBS = ["mosync_sdl"] + @LOCAL_LIBS
	if(HOST == :win32)
		@LIBRARIES += ["OpenGL32", "GlU32", "Gdi32"]
		@EXTRA_OBJECTS = [FileTask.new("mosynclib.def")]
	end

	@NAME = "mosync"
end

NativeMoSyncLib.new do
	@SOURCE_FILES = ["mosyncmain.cpp"]
	@NAME = "mosyncmain"
end

Works.run
