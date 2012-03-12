# Copyright (C) 2009 Mobile Sorcery AB
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License, version 2, as published by
# the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to the Free
# Software Foundation, 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# load local_config.rb, if it exists.
lc = "#{File.dirname(__FILE__)}/local_config.rb"
require lc if(File.exists?(lc))

require "#{File.dirname(__FILE__)}/host.rb"
require "#{File.dirname(__FILE__)}/task.rb"
require "#{File.dirname(__FILE__)}/gcc_flags.rb"
require "#{File.dirname(__FILE__)}/loader_md.rb"
require "#{File.dirname(__FILE__)}/flags.rb"

default_const(:PRINT_GCC_VERSION_INFO, false)

def get_gcc_version_info(gcc)
	puts "get_gcc_version_string(#{gcc})" if(PRINT_GCC_VERSION_INFO)
	info = {}
	file = open("|#{gcc} -v 2>&1")
	file.each do |line|
		parts = line.split(/ /)
		#puts "yo: #{parts.inspect}"
		if(parts[0] == 'Target:' && parts[1].strip == 'arm-elf')
			info[:arm] = true
		end
		if(parts[0] == "gcc" && parts[1] == "version")
			info[:ver] = parts[2].strip
		elsif(parts[0] == 'clang' && parts[1] == 'version')
			info[:clang] = true
			info[:ver] = parts[2].strip
		end
	end
	error("Could not find gcc version.") if(!info[:ver])
	info[:string] = ''
	info[:string] << 'arm-' if(info[:arm])
	info[:string] << 'clang-' if(info[:clang])
	info[:string] << info[:ver]
	return info
end

#warning("GCC version: #{GCC_VERSION}")
#warning("GCC_IS_V4: #{GCC_IS_V4}")
#warning("GCC_IS_V43: #{GCC_IS_V43}")

# Compiles a source file using gcc.
# Generates extra files for tracking dependencies and flags,
# so that if the flags or any dependency have changed, this file will be recompiled.
# Objects of this class are created by GccWork.
class CompileGccTask < FileTask
	def initialize(work, name, source, cflags)
		super(work, name)
		@SOURCE = source
		@prerequisites << source

		@DEPFILE = @work.genfile(source, ".mf")
		@TEMPDEPFILE = @work.genfile(source, ".mft")
		depFlags = " -MMD -MF \"#{@TEMPDEPFILE}\""
		@FLAGS = cflags + depFlags

		initFlags

		# only if the file is not already needed do we care about extra dependencies
		if(!needed?(false)) then
			@prerequisites += MakeDependLoader.load(@DEPFILE, @NAME)
		end
	end

	def needed?(log = true)
		return true if(super(log))
		if(!File.exists?(@DEPFILE))
			puts "Because the dependency file is missing:" if(log)
			return true
		end
		return flagsNeeded?(log)
	end

	def cFlags
		return "#{@FLAGS} #{@work.gccmode} \"#{File.expand_path_fix(@SOURCE)}\""
	end

	def execute
		execFlags
		begin
			sh "#{@work.gcc} -o \"#{@NAME}\"#{cFlags}"
		rescue => e
			# in case gcc output a broken object file
			FileUtils.rm_f(@NAME)
			raise
		end

		# In certain rare cases (error during preprocess caused by a header file)
		# gcc may output an empty dependency file, resulting in an empty dependency list for
		# the object file, which means it will not be recompiled, even though it should be.
		FileUtils.mv(@TEMPDEPFILE, @DEPFILE)
	end

	include FlagsChanged
end

# implementations of GccWork should include this module and implement gccVersionClass.
module GccVersion
	def set_defaults
		if(!gccVersionClass.class_variable_defined?(:@@GCC_IS_V4))
			gcc_version_info = get_gcc_version_info(gcc)
			@@gcc_version_info = gcc_version_info
			is_v4 = gcc_version_info[:ver][0] == "4"[0]
			set_class_var(gccVersionClass, :@@GCC_IS_V4, is_v4)
			if(is_v4)
				set_class_var(gccVersionClass, :@@GCC_V4_SUB, gcc_version_info[:ver][2, 1].to_i)
			end

			warning("GCC version: #{gcc_version_info.inspect}") if(PRINT_GCC_VERSION_INFO)
			#warning("GCC_IS_V4: #{is_v4}")
			if(is_v4)
				#warning("GCC sub-version: #{gcc_version_info[:ver][2, 1].to_i}")
			end

			# Assuming for the moment that clang is command-line-compatible with gcc 4.2.
			isClang = gcc_version_info[:clang]
			if(isClang)
				set_class_var(gccVersionClass, :@@GCC_IS_V4, true)
				set_class_var(gccVersionClass, :@@GCC_V4_SUB, 2)
			end
			set_class_var(gccVersionClass, :@@GCC_IS_CLANG, isClang)
			set_class_var(gccVersionClass, :@@GCC_IS_ARM, gcc_version_info[:arm])
		end
		@gcc_version_info = @@gcc_version_info
		@GCC_IS_CLANG = get_class_var(gccVersionClass, :@@GCC_IS_CLANG)
		@GCC_IS_ARM = get_class_var(gccVersionClass, :@@GCC_IS_ARM)
		@GCC_IS_V4 = get_class_var(gccVersionClass, :@@GCC_IS_V4)
		if(@GCC_IS_V4)
			@GCC_V4_SUB = get_class_var(gccVersionClass, :@@GCC_V4_SUB)
		end

		@GCC_WNO_UNUSED_BUT_SET_VARIABLE = ''
		@GCC_WNO_POINTER_SIGN = ''
		if(@GCC_IS_V4 && @GCC_V4_SUB >= 6)
			@GCC_WNO_UNUSED_BUT_SET_VARIABLE = ' -Wno-unused-but-set-variable'
			@GCC_WNO_POINTER_SIGN = ' -Wno-pointer-sign'
		end
		super
	end
end

# Base class.
# Compiles C/C++ code into an executable file.
# Supports GCC on mingw, pipe and linux.
# Uses the following variables: @SOURCES, @IGNORED_FILES, @EXTRA_SOURCEFILES,
# @SPECIFIC_CFLAGS and @EXTRA_OBJECTS.
# Requires subclasses to implement methods 'gcc', 'gccmode' and 'object_ending'.
class GccWork < BuildWork
	# Returns a path representing a generated file, given a source filename and a new file ending.
	def genfile(source, ending)
		@BUILDDIR + File.basename(source.to_s).ext(ending)
	end

	# The filename of the target.
	def target
		@TARGET
	end

	private

	include GccFlags

	def setup2
		define_cflags
		@CFLAGS_MAP = { ".c" => @CFLAGS + host_flags,
			".cpp" => @CPPFLAGS + host_flags + host_cppflags,
			".cc" => @CPPFLAGS + host_flags + host_cppflags }

		#find source files
		cfiles = collect_files(".c")
		cppfiles = collect_files(".cpp") + collect_files(".cc")

		if(USE_ARM)
			@CFLAGS_MAP['.S'] = @CFLAGS + host_flags
			sfiles = collect_files('.S')
			cfiles += sfiles
		end

		if(HOST == :darwin)
			@CFLAGS_MAP[".mm"] = @CPPFLAGS + host_flags + host_cppflags
			cppfiles += collect_files(".mm")
		end

		@all_sourcefiles = cfiles + cppfiles
		@all_sourcefiles.sort! do |a,b| b.timestamp <=> a.timestamp end

		@source_objects = objects(@all_sourcefiles)
		all_objects = @source_objects + @EXTRA_OBJECTS

		setup3(all_objects, !cppfiles.empty?)
 	end

	def check_extra_sourcefile(file, ending)
		return false if(file.getExt != ending)
		raise "Extra sourcefile '#{file}' does not exist!" if(!File.exist?(file))
		return true
	end

	# returns an array of FileTasks.
	# is case-sensitive in the file-ending comparison
	def collect_files(ending)
		files = @SOURCES.collect {|dir| Dir[dir+"/*"+ending]}
		files.flatten!
		files.reject! {|file| @IGNORED_FILES.member?(File.basename(file))}
		files.reject! {|file| file.to_s.getExt != ending}
		files += @EXTRA_SOURCEFILES.select do |file| check_extra_sourcefile(file, ending) end
		tasks = files.collect do |file| FileTask.new(self, file) end
		extra_tasks = @EXTRA_SOURCETASKS.select do |file| file.to_s.getExt == ending end
		return extra_tasks + tasks
	end

	def getGccFlags(source)
		ext = source.to_s.getExt
		cflags = @CFLAGS_MAP[ext]
		if(cflags == nil) then
			error "Bad ext: '#{ext}' from source '#{source}'"
		end
		need(:@SPECIFIC_CFLAGS)
		cflags += @SPECIFIC_CFLAGS.fetch(File.basename(source.to_s), "")
		return cflags
	end

	def makeGccTask(source, ending)
		objName = genfile(source, ending)
		task = CompileGccTask.new(self, objName, source, getGccFlags(source))
		return task
	end

	# returns an array of CompileGccTasks
	def objects(sources)
		return sources.collect do |path| makeGccTask(path, object_ending) end
	end
end
