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

require "#{File.dirname(__FILE__)}/native_gcc.rb"
require "#{File.dirname(__FILE__)}/flags.rb"
require "#{File.dirname(__FILE__)}/dynlibconv.rb"

# Base class.
# Links object files together to form an native executable or shared library.
class NativeGccLinkTask < FileTask
	def initialize(work, name, objects, linker)
		super(work, name)
		initFlags
		@prerequisites += @objects = objects
		@linker = linker
	end

	def cFlags
		if(@work.target_platform == :linux)
			obj = ''
			@objects.each do |o|
				o = o.to_s
				# This should make shared libraries easier to distribute.
				if(o.end_with?('.so') && o.start_with?('lib'))
					lib = File.basename(o).slice(3 .. -4)
					obj << " \"-L#{File.dirname(o)}\" -l#{lib}"
				else
					obj << " \"#{o}\""
				end
			end
		else
			obj = "\"#{@objects.join('" "')}\""
		end
		return "#{obj}#{@FLAGS}"
	end

	def execute
		execFlags
		# Use a temporary name so the file gets rebuilt in case of error in the DynLibConv step.
		tmpName = @NAME
		tmpName += '.tmp' if(@work.target_platform == :darwin)
		sh "#{@linker} #{cFlags} -o \"#{tmpName}\""
		if(@work.target_platform == :darwin)
			DynLibConv.run("/opt/local/lib", "@loader_path", tmpName)
			FileUtils.mv(tmpName, @NAME)
		end
	end

	include FlagsChanged
end

# Base class.
# Compiles C/C++ code into an native executable file or shared library.
# Supports GCC on mingw and linux.
# In addition to the variables used by GccWork, this class uses the following:
# @NAME, @LOCAL_LIBS, @LOCAL_DLLS, @WHOLE_LIBS, @LIBRARIES, @COMMON_BUILDDIR, @BUILDDIR and @TARGETDIR.
class NativeGccLinkWork < NativeGccWork
	private

	def linkerName(have_cppfiles); have_cppfiles ? 'g++' : 'gcc'; end
	def applyLibraries
		@LIBRARIES.each { |l| @EXTRA_LINKFLAGS += " -l" + l }
	end
	def setup3(all_objects, have_cppfiles)
		if(@TARGET_PLATFORM == :darwin)
			@EXTRA_LINKFLAGS += " -L/opt/local/lib -framework Cocoa -framework IOBluetooth -framework Foundation"
		end
		if(PROFILING)
			@EXTRA_LINKFLAGS += " -pg"
		end
		llo = @LOCAL_LIBS.collect { |ll| FileTask.new(self, @COMMON_BUILDDIR + ll + ".a") }
		lldPrefix = ''
		lldPrefix = 'lib' if(@TARGET_PLATFORM == :linux)
		lld = @LOCAL_DLLS.collect { |ld| FileTask.new(self, @COMMON_BUILDDIR + lldPrefix + ld + DLL_FILE_ENDING) }
		wlo = @WHOLE_LIBS.collect { |ll| FileTask.new(self, @COMMON_BUILDDIR + ll + ".a") }
		applyLibraries
		need(:@NAME)
		need(:@BUILDDIR)
		need(:@TARGETDIR)
		target = @TARGETDIR + "/" + @BUILDDIR + @NAME + link_file_ending
		#puts "@EXTRA_LINKFLAGS: "+@EXTRA_LINKFLAGS
		@TARGET = link_task_class.new(self, target, all_objects, wlo, llo + lld, @EXTRA_LINKFLAGS, linkerName(have_cppfiles))
		@prerequisites += [@TARGET]
	end
end
