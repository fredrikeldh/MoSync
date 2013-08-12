# Include this file when building MoSync code.

require "#{File.dirname(__FILE__)}/mosync_config.rb"
require "#{File.dirname(__FILE__)}/mosync.rb"
require "#{File.dirname(__FILE__)}/cExe.rb"
require "#{File.dirname(__FILE__)}/mosyncGccModule.rb"

class MoSyncExe < ExeWork
	include MoSyncInclude

	def initialize(compilerModule = DefaultMoSyncCCompilerModule, &block)
		super(compilerModule) do
			instance_eval(&block)
			# String, GCC flags describing the default include directories.
			default(:DEFAULT_INCLUDES, " -I\"#{mosync_include}\"")

			@EXTRA_CFLAGS ||= ''
			@EXTRA_CFLAGS += @DEFAULT_INCLUDES
			@EXTRA_CPPFLAGS ||= ''
			@EXTRA_CPPFLAGS += @DEFAULT_INCLUDES
		end
	end
end
