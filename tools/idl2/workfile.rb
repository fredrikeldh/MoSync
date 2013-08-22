#!/usr/bin/ruby

require File.expand_path('../../rules/cExe.rb')

WORK = ExeWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["../../intlibs"]
	@LOCAL_LIBS = ["idl-common", "filelist"]
	@NAME = "idl2"
	@TARGETDIR = "."
end

target :default do
end

class CompileTask < FileTask
	def initialize
		@prerequisites = [DirTask.new('../../runtimes/java/Shared/generated')]
		@prerequisites += [WORK] + (["extensions.h", "maapi_defs.h"] + Dir["*.idl", "**/*.idl"]).collect do |f|
			FileTask.new(f)
		end
		# let's pick an early target, for max speed.
		super("Output/invoke_syscall_cpp.h")
	end
	def fileExecute
		sh WORK.to_s
	end
end

target :compile => :default do
	begin
		ct = CompileTask.new
		# fails if idl2.exe needs to be regenerated, because at the time of task instantiation, the prereq is still needed,
		# but at the time of run (which is essentially the same time), the prereq has already been executed, so it won't call the needy task.
		Works.run(false)
	rescue => e
		# if the compiler should fail, we must make sure it runs next time.
		FileUtils.rm_f(ct.to_str)
		raise
	end
end

target :clean do
	verbose_rm_rf("build")
	verbose_rm_rf("Output")
	#TODO: remove all copied files
end

Works.run
