require "#{File.dirname(__FILE__)}/cExe.rb"
require "#{File.dirname(__FILE__)}/cDll.rb"
require "#{File.dirname(__FILE__)}/cLib.rb"

module NativeMoSyncWork
	def set_defaults
		super
		# Array of Strings, names of Win32 MoSync precompiled libraries to link with.
		default(:CUSTOM_LIBS, [])
	end
	def define_cflags
		bd = File.expand_path_fix(File.dirname(__FILE__) + "/..")
		if(@TARGET_PLATFORM == :win32)
			@EXTRA_INCLUDES += [bd+"/tools/ReleasePackageBuild/build_package_tools/include"]
			custom_lib_dir = bd+"/tools/ReleasePackageBuild/build_package_tools/lib/"
			need(:@CUSTOM_LIBS)
			@CUSTOM_LIBS.each { |cl| @EXTRA_LINKFLAGS += " " + custom_lib_dir + cl }
		elsif(@TARGET_PLATFORM == :darwin)
			@EXTRA_INCLUDES << '/opt/local/include'
		end
		@EXTRA_INCLUDES += [bd+"/intlibs", bd+"/libs"]
		@EXTRA_CFLAGS += " -D_POSIX_SOURCE -DMOSYNC_NATIVE"	#avoid silly bsd functions
	end
end

class NativeMoSyncLib < LibWork
	include NativeMoSyncWork
end

class MoSyncExe < ExeWork
	include NativeMoSyncWork
end

class MoSyncDll < DllWork
	include NativeMoSyncWork
end
