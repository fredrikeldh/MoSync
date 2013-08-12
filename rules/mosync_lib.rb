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

			# Array of Strings, names of directories containing header files to copy into mosync_include/@HEADER_INSTALLDIR/d.
			default(:HEADER_DIRS, @SOURCES)
			# Array of Strings, paths of directories containing header files to copy into mosync_include/@HEADER_INSTALLDIR/basename(d).
			default(:HEADER_FLAT_DIRS, [])
			# Array of Strings, paths of directories containing header files to copy into mosync_include/@HEADER_INSTALLDIR.
			default(:HEADER_MERGE_DIRS, [])
			# String, name of subdirectory of mosync_include, where header files will be installed. Required if @HEADER_DIRS is not empty.
			default(:HEADER_INSTALLDIR, nil)
			# Array of Strings, name patterns of header files. Only files matching one of these patterns will be copied.
			default(:HEADER_FILE_PATTERNS, ['*.h', '*.hpp'])
			# String, GCC flags describing the default include directories.
			default(:DEFAULT_INCLUDES, " -I\"#{mosync_include}\"")

			@EXTRA_CFLAGS ||= ''
			@EXTRA_CFLAGS += @DEFAULT_INCLUDES
			@EXTRA_CPPFLAGS ||= ''
			@EXTRA_CPPFLAGS += @DEFAULT_INCLUDES

			if(!(@HEADER_DIRS.empty? && @HEADER_FLAT_DIRS.empty? && @HEADER_MERGE_DIRS.empty?))
				need(:@HEADER_INSTALLDIR)
				@REQUIREMENTS ||= []
				patterns = @HEADER_FILE_PATTERNS
				@HEADER_DIRS.each do |name|
					#puts "HEADER_DIR #{name}"
					patterns.each do |e|
						@REQUIREMENTS << CopyDirTask.new(mosync_include + '/' + @HEADER_INSTALLDIR, name, name, false, e)
					end
				end
				@HEADER_FLAT_DIRS.each do |name|
					#puts "HEADER_FLAT_DIR #{name}"
					patterns.each do |e|
						@REQUIREMENTS << CopyDirTask.new(mosync_include + '/' + @HEADER_INSTALLDIR, File.basename(name), name, false, e)
					end
				end
				@HEADER_MERGE_DIRS.each do |name|
					#puts "HEADER_MERGE_DIR #{name}"
					patterns.each do |e|
						@REQUIREMENTS << CopyDirTask.new(mosync_include, @HEADER_INSTALLDIR, name, false, e)
					end
				end
			end
		end
	end
end
