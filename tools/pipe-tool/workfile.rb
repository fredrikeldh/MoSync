#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')
require File.expand_path('../../rules/mosync_util.rb')

class ProtoTask < FileTask
	def initialize(cb)
		@prerequisites = Dir['*.c'].collect do |fn| FileTask.new(fn); end
		@prerequisites << FileTask.new(cb+'protobuild'+HOST_EXE_FILE_ENDING)
		@cb = cb
		super('PBProto.h')
	end
	def fileExecute
		sh "#{@cb}protobuild"
	end
end

NativeMoSyncExe.new do
	@SOURCES = ["."]
	@IGNORED_FILES = ["Emu.c", "BrewRebuild.c", "Peeper.c", "JavaCodeGen.c", "disas.c"]

	@EXTRA_CFLAGS = " -Wno-strict-prototypes -Wno-missing-prototypes -Wno-old-style-definition" +
		" -Wno-missing-noreturn -Wno-shadow -Wno-unreachable-code -Wno-write-strings -Wno-multichar" +
		" -Wno-missing-format-attribute -D_CRT_SECURE_NO_DEPRECATE -DUSE_ZLIB -fno-strict-aliasing -m32"
	@EXTRA_LINKFLAGS = " -m32"
	# -Wno-unused-function
	@LIBRARIES = ["z"]
	@NAME = "pipe-tool"
	@INSTALLDIR = mosyncdir + '/bin'

	def set_defaults
		super
		@REQUIREMENTS = [ProtoTask.new(@COMMON_BUILDDIR)]
	end
end

Works.run
