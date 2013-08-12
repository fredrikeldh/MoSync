require "#{File.dirname(__FILE__)}/cCompile.rb"
require "#{File.dirname(__FILE__)}/host.rb"

class ExeWork < CCompileWork
	def cFlags
		return @cFlags if(@cFlags)
		return @cFlags = linkCmd + objectFlags
	end
	def targetName()
		return CCompileTask.genFilename(@COMMON_EXE ? @COMMON_BUILDDIR : @BUILDDIR, @NAME, HOST_EXE_FILE_ENDING)
	end
end
