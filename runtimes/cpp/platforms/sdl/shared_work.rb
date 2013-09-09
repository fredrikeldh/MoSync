require File.expand_path('../../../../../rules/arg_handler.rb')

Works.registerConstArg(:NATIVE_RUNTIME, false)
Works.registerConstArg(:FULLSCREEN, false)

require File.expand_path('../../../../../rules/native_mosync.rb')

Works.parseArgs()

module SdlCommon
def setup_common
	common_includes = ["../../../base", ".."]

	@LOCAL_LIBS = ["mosync_log_file", "mosync_bluetooth", "net", "filelist", "dll", "sqlite"]
	@LOCAL_DLLS = ['dgles']
	common_libraries = ["SDL", "SDL_image", "SDL_ttf"]

	if(HOST == :win32) then
		@CUSTOM_LIBS = common_libraries.collect do |lib| "#{lib}.lib" end +
			["libexpat.lib", "SDL_sound.lib", "libbthprops_ex.a", "libuuid.a", "FreeImage.lib",
				"libeay32.lib", "ssleay32.lib"]
		@LIBRARIES = ["wsock32", "ws2_32", "Opengl32"]
		@LOCAL_DLLS << 'gsm_amr'
		@EXTRA_INCLUDES = ["../../../base", ".."]
		@EXTRA_LINKFLAGS = ' -Wl,--enable-stdcall-fixup'
	elsif(HOST == :linux) then
		@EXTRA_CPPFLAGS = ""
		@IGNORED_FILES = []
		@LOCAL_LIBS << "gsm_amr"
		if(HOST_HAS_SDL_SOUND)
			sound_lib = [ "SDL_sound" ]
		else
			sound_lib = []
			@EXTRA_CPPFLAGS += " -D__NO_SDL_SOUND__"
			@IGNORED_FILES += [ "SDLSoundAudioSource.cpp" ]
		end

		if(FULLSCREEN == "true")
			@EXTRA_CPPFLAGS += " -D__USE_FULLSCREEN__"
		end
		@LIBRARIES = common_libraries + sound_lib + ["gtk-x11-2.0", "bluetooth", "expat", "freeimage",
			"ssl", "crypto", "dl", "pthread", "gobject-2.0", 'GL', 'GLEW']
		@EXTRA_INCLUDES = ["../../../base", ".."]

	elsif(HOST == :darwin)
		@LOCAL_LIBS << "gsm_amr"
		@LIBRARIES = common_libraries + ["SDL_sound", "SDLmain", "expat", "freeimage", "ssl", "crypto", 'GLEW']
		@EXTRA_INCLUDES = common_includes + ["/opt/local/include"]
	else
		error "Unsupported platform"
	end
end
end
