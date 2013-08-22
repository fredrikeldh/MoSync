require "#{File.dirname(__FILE__)}/gccModule.rb"

module MoSyncGccCompilerModule
	include GccCompilerModule
	def gcc
		mosyncdir + '/mapip2/xgcc-4.6.3' + HOST_EXE_FILE_ENDING
	end
	def linkerName
		gcc
	end
	def isPipeWork
		true
	end
	def setCompilerVersion
		default(:TARGET_PLATFORM, :mapip2)
		super
	end
	def customTargetSetFlags
		return ' -DMAPIP', ''
	end

	private
	def libTask(lib)
		return FileTask.new("#{mosync_libdir}/#{@BUILDDIR_NAME}/#{lib}#{HOST_LIB_FILE_ENDING}")
	end
	def libTasks
		return (@LIBRARIES + @DEFAULT_LIBS).collect do |lib|
			libTask(lib)
		end
	end

	public
	def linkCmd
		flags = ' -nodefaultlibs -nostartfiles -Wl,--warn-common,--emit-relocs,--no-check-sections'
		flags << ' -Wl,--start-group'
		libs = libTasks
		libs.each do |l|
			flags << " \"#{l}\""
		end
		flags << ' -Wl,--end-group'

		flags << @EXTRA_LINKFLAGS

		raise hell if(@LIBRARIES.uniq.length != @LIBRARIES.length)
		raise hell if(@object_tasks.uniq.length != @object_tasks.length)
		return "#{linkerName} -o \"#{@NAME}\" @#{objectsFileName}#{flags}"
	end
end
