#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

NativeMoSyncLib.new do
	if(HOST == :linux)
		@SOURCES = [".", "linux", "linux/bluez"]
		if(HOST_HAS_BLUETOOTH)
			@EXTRA_CPPFLAGS = " -DBLUEZ_SUPPORTED"
		else
			#error "libbluetooth-dev missing!"
		end

		@EXTRA_INCLUDES = ["../../runtimes/cpp/base", "../../runtimes/cpp/platforms/sdl"]
		@SPECIFIC_CFLAGS = { "interface.cpp" => " -Wno-missing-noreturn", "discovery.cpp" => " -Wno-missing-noreturn" }
	elsif(HOST == :win32)
		@SOURCES = [".", "win32"]
		if(!@GCC_IS_V4)
			@SPECIFIC_CFLAGS = { "discImpl.cpp" => " -Wno-unreachable-code" }
		end
	elsif(HOST == :darwin)
		@SOURCES = [".", "darwin"]
		@EXTRA_CPPFLAGS = " -DCOCOA_SUPPORTED"
		@EXTRA_INCLUDES = ["../../runtimes/cpp/base", "../../runtimes/cpp/platforms/sdl"]
		@SPECIFIC_CFLAGS = { "interface.mm" => " -Wno-missing-noreturn","discovery.cpp" => " -Wno-missing-noreturn" }
	else
		error "Unknown platform: #{HOST}"
	end

	@NAME = "mosync_bluetooth"

	@REQUIREMENTS = [CopyFileTask.new("config_bluetooth.h", FileTask.new("config_bluetooth.h.example"))]
end

Works.run
