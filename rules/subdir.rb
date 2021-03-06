require "#{File.dirname(__FILE__)}/arg_handler.rb"

Works.registerConstArg(:CONFIG_SUBDIR_RELOAD, false)

class Works
	def self.invoke_subdir(dir, *args)
		self.invoke_subdir_ex(false, dir, *args)
	end
	def self.invoke_subdir_ex(reload, dir, *args)
		self.invoke_subdir_ex2('workfile.rb', reload, dir, *args)
	end
	def self.invoke_subdir_ex2(workfileName, reload, dir, *args)
		@@handlers.each do |name, proc|
			if(@@args_handled[name])
				args << "#{name}=\"#{@@args_handled[name]}\""
			end
		end
		puts File.expand_path(dir) + " " + args.inspect
		fn = dir + '/' + workfileName
		if(!File.exists?(fn))
			raise "No #{workfileName} found in #{dir}"
		end

		oldDir = Dir.getwd
		Dir.chdir(dir)
		if(CONFIG_SUBDIR_RELOAD || reload)
			cmd = "#{File.expand_path(workfileName)} #{args.join(' ')}"
			sh "ruby -r\"#{File.dirname(__FILE__)}/subdir_arg_handler.rb\" #{cmd}"
		else
			@@ignore_unhandled_args = true
			oldHandlers = @@handlers
			oldArgs = @@args_handled
			resetTargets(args)
			load(File.expand_path(workfileName), true)
			@@handlers = oldHandlers
			@@args_handled = oldArgs
		end
		if(CONFIG_CHECK_TASK_INTEGRITY)
			sh "ruby workfile.rb TASK_TEST_RUN="
		end
		Dir.chdir(oldDir)
	end

	def self.invoke_subdirs(dirs, *args)
		self.invoke_subdirs_ex(false, dirs, *args)
	end
	def self.invoke_subdirs_ex(reload, dirs, *args)
		dirs.each do |dir| invoke_subdir_ex(reload, dir, *args) end
	end
end
