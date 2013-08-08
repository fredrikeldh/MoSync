#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@SOURCE_FILES = [
		'../../runtimes/cpp/base/FileStream.cpp',
		'../../runtimes/cpp/base/ThreadPool.cpp',
		'../../runtimes/cpp/platforms/sdl/FileImpl.cpp',
		'../../runtimes/cpp/platforms/sdl/ThreadPoolImpl.cpp',
	]
	@EXTRA_INCLUDES = ['../../intlibs', '../../runtimes/cpp/base', '../../runtimes/cpp/platforms/sdl']
	@LOCAL_LIBS = ['mosync_bluetooth', 'mosync_log_file']

	common_libraries = ['SDL', 'SDLmain']
	if(HOST == :win32) then
		@CUSTOM_LIBS = common_libraries.collect do |lib| "#{lib}.lib" end +
			['libirprops.a', 'libuuid.a']
		@LIBRARIES = ['wsock32', 'ws2_32']
	elsif(HOST == :linux) then
		@LIBRARIES = common_libraries + ['bluetooth', 'pthread']
		@SOURCE_FILES << '../../runtimes/cpp/platforms/sdl/mutexImpl.cpp'
	elsif(HOST == :darwin)
		@LIBRARIES = common_libraries
		@SOURCE_FILES << '../../runtimes/cpp/platforms/sdl/mutexImpl.cpp'
	else
		error 'Unsupported platform'
	end

	@NAME = 'unitTestSocketServer'
end

Works.run
