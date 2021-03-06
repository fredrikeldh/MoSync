#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')
require './shared.rb'

MoSyncLib.new do
	extend MAStd
	initMAStd

	@IGNORED_FILES = []
	if(@CONFIG == "release" && @GCC_IS_V4)
		#broken compiler/stdlib
		native_specflags = {"conprint.c" => " -Wno-unreachable-code"}
		if(@GCC_V4_SUB == 4)
			native_specflags["conprint.c"] += " -Wno-inline"
		end
	else
		native_specflags = {}
	end
	if(@CONFIG == "release")
		pipe_specflags = {"strtod.c" => " -Wno-unreachable-code",
			"tlsf.c" => " -Wno-unreachable-code"}
		# broken compiler
		native_specflags["mastdlib.c"] = " -Wno-unreachable-code"
		native_specflags["mastring.c"] = " -Wno-unreachable-code"
		native_specflags["mawstring.c"] = " -Wno-unreachable-code"
		native_specflags["mavsprintf.c"] = " -Wno-unreachable-code"
		native_specflags["mawvsprintf.c"] = " -Wno-unreachable-code"
	else
		pipe_specflags = {}
	end
	if(@GCC_IS_V4)
		pipe_specflags['mastring.c'] = ' -Wno-pointer-sign'
	end

	@native_specific_cflags = {
		"madmath.c" => " -Wno-missing-declarations",
		"mavsprintf.c" => " -Wno-float-equal -Wno-missing-format-attribute",
		"mawvsprintf.c" => " -Wno-float-equal",
		'VariantResourceLookup.cpp' => ' -Wno-vla',
		}.merge(native_specflags, &HashMergeAdd)

	@pipe_specific_cflags = @native_specific_cflags.merge({
		"intrinsics.c" => " -Wno-missing-prototypes -Wno-missing-declarations",
		"madmath.c" => " -Wno-missing-prototypes -Wno-missing-declarations -fno-builtin-isnan",
		"maint64.c" => " -fno-strict-aliasing -Wno-missing-prototypes -Wno-missing-declarations",
		"libgcc2.c" => " -Wno-unreachable-code",
		"strtod.c" => " -Wno-float-equal" + @GCC_WNO_UNUSED_BUT_SET_VARIABLE,
		"e_log.c" => " -Wno-float-equal",
		"s_atan.c" => " -fno-strict-aliasing",
		"e_atan2.c" => " -fno-strict-aliasing",
		"e_asin.c" => " -fno-strict-aliasing",
		"mastdlib.c" => " -Wno-deprecated-declarations",
		'maheap.c' => ' -Wno-c++-compat',
		}, &HashMergeAdd).merge(pipe_specflags, &HashMergeAdd)

	@SOURCES = [".", "../libsupc++", "libgcc", "../ResCompiler"]
	@EXTRA_INCLUDES = ['.']
	@DEFAULT_INCLUDES = ''
	@SPECIFIC_CFLAGS = @pipe_specific_cflags

	@IGNORED_FILES << 'e_pow.c'
	@IGNORED_FILES << 'e_log.c'
	@IGNORED_FILES << 'e_atan2.c'
	@HEADER_DIRS = [
		'GLES',
		'GLES2',
		'bits',
		'.',
	]
	@HEADER_INSTALLDIR = '.'
	@IGNORED_HEADERS = ["math_private.h", "fdlibm.h"]

	@NAME = "mastd"
end

Works.run
