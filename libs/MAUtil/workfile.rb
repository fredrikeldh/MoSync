#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ["."]
	@EXTRA_SOURCEFILES = ["../kazlib/dict.c", "../kazlib/hash.c"]
	@EXTRA_INCLUDES = ['..']
	@HEADER_DIRS = [
		'../kazlib',
		'.',
	]
	@HEADER_INSTALLDIR = "MAUtil"
	@NAME = "mautil"
	@IGNORED_FILES = ["DomParser.cpp", "XMLDataProvider.cpp", "XPathTokenizer.cpp"]
	@IGNORED_HEADERS = ["DomParser.h", "XMLDataProvider.h", "XPathTokenizer.h", "Tokenizer.h", "ErrorListenable.h"]

	if(@CONFIG == "release")
		# broken compiler
		@SPECIFIC_CFLAGS = {
			"CharInputC.c" => " -Wno-unreachable-code",
			"Graphics.c" => " -Wno-unreachable-code",
			"GraphicsOpenGL.c" => " -Wno-unreachable-code",
			"GraphicsSoftware.c" => " -Wno-unreachable-code",
			"FrameBuffer.c" => " -Wno-unreachable-code",
		}
		if(@GCC_IS_V4)
			@SPECIFIC_CFLAGS["String.cpp"] = " -Wno-strict-overflow"
		end
	else
		raise hell if(@CONFIG != 'debug')
		@SPECIFIC_CFLAGS = {}
	end
	@SPECIFIC_CFLAGS["dict.c"] = " -Wno-unreachable-code"
	@SPECIFIC_CFLAGS["hash.c"] = " -Wno-unreachable-code"
end

Works.run
