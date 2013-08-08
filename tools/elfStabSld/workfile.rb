#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')
require File.expand_path('../../rules/mosync_util.rb')
require 'stringio'

require './stabdefs.rb'

class GenStabDefsH < MemoryGeneratedFileTask
	def initialize()
		io = StringIO.new
		first = true
		io.puts 'const char* stabName(unsigned char type);'
		io.puts
		io.puts '#define N_EXT 0x1'
		io.puts
		STABS.each do |s|
			if(s.name.include?('|'))
				raise hell if(!s.name.end_with?(' | N_EXT'))
				next
			end
			io.puts "#define #{s.name} #{s.type}"
		end
		@buf = io.string
		@prerequisites = [DirTask.new('build')]
		super('build/stabdefs.h')
	end
end

class GenStabDefsC < MemoryGeneratedFileTask
	def initialize()
		io = StringIO.new
		first = true
		io.puts '#include "stabdefs.h"'
		io.puts '#include <stdio.h>'
		io.puts '#include <stdlib.h>'
		io.puts
		io.puts 'const char* stabName(unsigned char type) {'
		io.puts "\tswitch(type) {"
		STABS.each do |s|
			io.puts "\t\tcase #{s.type}: return \"#{s.name}\";"
		end
		io.puts "\t\tdefault: printf(\"Unknown type 0x%02x\\n\", type); exit(1); return \"\";"
		io.puts "\t}"
		io.puts '}'
		@buf = io.string
		@prerequisites = [DirTask.new('build')]
		super('build/stabdefs.cpp')
	end
end

class GenRegnamesTask < FileTask
	def initialize
		@gen = '../../runtimes/cpp/core/gen-opcodes.rb'
		@prerequisites = [FileTask.new(@gen)]
		super('build/gen-regnames.h')
	end
	def fileExecute
		sh "ruby #{@gen} regnames #{@NAME}"
	end
end

MoSyncExe.new do
	@SOURCES = ['.']
	@SOURCE_FILES = [
		'../../runtimes/cpp/platforms/sdl/FileImpl.cpp',
		'../../runtimes/cpp/base/FileStream.cpp',
	]
	@REQUIREMENTS = [
		GenStabDefsH.new,
		GenRegnamesTask.new,
	]
	@SOURCE_TASKS = [
		GenStabDefsC.new,
	]
	@EXTRA_INCLUDES = [
		'../../intlibs',
		'../../runtimes/cpp/base',
		'../../runtimes/cpp/platforms/sdl',
	]
	@LOCAL_LIBS = ['mosync_log_file']
	if(HOST == :linux)
		@LIBRARIES = ['pthread']
	end

	@NAME = 'elfStabSld'

	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
