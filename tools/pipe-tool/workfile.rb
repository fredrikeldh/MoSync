#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')
require File.expand_path('../../rules/mosync_util.rb')

class ProtoTask < FileTask
	def initialize
		@prerequisites = Dir['*.c'].collect do |fn| FileTask.new(fn); end
		super('PBProto.h')
	end
	def fileExecute
		sh "#{WORK.cb}protobuild"
	end
end

WORK = MoSyncExe.new do
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
	@REQUIREMENTS = [ProtoTask.new]

	def cb
		@COMMON_BUILDDIR
	end
end

Works.run
