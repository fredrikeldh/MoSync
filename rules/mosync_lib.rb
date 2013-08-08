# Include this file when building MoSync code.

require "#{File.dirname(__FILE__)}/mosync_config.rb"
require "#{File.dirname(__FILE__)}/mosync.rb"
require "#{File.dirname(__FILE__)}/cLib.rb"
require "#{File.dirname(__FILE__)}/mosyncGccModule.rb"

class MoSyncLib < LibWork
	include MoSyncInclude

	def copyHeaderFile(targetSubDir, srcFileName)
		tfn = mosync_include + '/' + targetSubDir + '/' + File.basename(srcFileName)
		dir = File.dirname(tfn)
		return CopyFileTask.new(tfn, FileTask.new(srcFileName), [DirTask.new(dir)])
	end

	def default(sym, val)
		if(sym == :LIB_TARGETDIR)
			val = mosync_libdir + '/' + @BUILDDIR_NAME + '/'
		end
		s = ('@'+sym.to_s).to_sym
		instance_variable_set(s, val) if(!instance_variable_defined?(s))
	end

	def initialize(compilerModule = DefaultMoSyncCCompilerModule, &block)
		super(compilerModule) do
			instance_eval(&block)
			# Array of Strings, names of directories containing header files to copy.
			default(:HEADER_DIRS, @SOURCES)
			# String, name of subdirectory of mosync_include, where header files will be installed. Required if @HEADER_DIRS is not empty.
			default(:HEADER_INSTALLDIR, nil)
			# String, GCC flags describing the default include directories.
			default(:DEFAULT_INCLUDES, " -I\"#{mosync_include}\"")

			@EXTRA_CFLAGS ||= ''
			@EXTRA_CFLAGS += @DEFAULT_INCLUDES
			@EXTRA_CPPFLAGS ||= ''
			@EXTRA_CPPFLAGS += @DEFAULT_INCLUDES

			if(!@HEADER_DIRS.empty?)
				need(:@HEADER_INSTALLDIR)
				@REQUIREMENTS ||= []
				endings = ['*.h', '*.hpp']
				@HEADER_DIRS.each do |name|
					endings.each do |e|
						@REQUIREMENTS << CopyDirTask.new(mosync_include + '/' + @HEADER_INSTALLDIR, name, name, false, e)
					end
				end
			end
		end
	end
end
