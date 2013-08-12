# Include this file when building MoSync code.

require "#{File.dirname(__FILE__)}/arg_handler.rb"

Works.registerConstArg(:USE_NEWLIB, false)
Works.registerConstArg(:USE_GNU_BINUTILS, true)

require "#{File.dirname(__FILE__)}/mosync_util.rb"
require "#{File.dirname(__FILE__)}/cCompile.rb"

Works.parseArgs

default_const(:MOSYNC_NATIVE, (defined?(MODE) && MODE == 'bb10'))
HAVE_LIBC = USE_NEWLIB || MOSYNC_NATIVE
default_const(:DEFAULT_BB10_ARCH, 'arm')
default_const(:BB10_ARCH, DEFAULT_BB10_ARCH)
if(USE_NEWLIB && MOSYNC_NATIVE)
	raise 'Newlib is not compatible with native libc.'
end

module MoSyncInclude
	def set_defaults
		if(USE_NEWLIB)
			default(:BUILDDIR_PREFIX, 'newlib-')
		end
		super
	end
	def define_cflags
		if(USE_NEWLIB)
			@EXTRA_CFLAGS += ' -DUSE_NEWLIB=1'
			@EXTRA_CPPFLAGS += ' -DUSE_NEWLIB=1'
		end
	end
	def mosync_include; "#{mosyncdir}/include" + sub_include; end
	def mosync_libdir; "#{mosyncdir}/lib"; end
	def sub_include; USE_NEWLIB ? "/newlib" : ""; end
	def use_stlport
		@EXTRA_INCLUDES ||= []
		@EXTRA_INCLUDES << "#{mosync_include}/stlport" if(!MOSYNC_NATIVE)
		@LIBRARIES ||= []
		@LIBRARIES << "stlport" if(!MOSYNC_NATIVE)
	end
end
