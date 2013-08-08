require "#{File.dirname(__FILE__)}/gccModule.rb"

module MoSyncGccCompilerModule
	include GccCompilerModule
	def gcc
		mosyncdir + '/mapip2/xgcc-4.6.3' + HOST_EXE_FILE_ENDING
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
end
