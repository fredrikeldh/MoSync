
def switchPathSlashes(cmd)
	raise "null path" if(!cmd)
	if(HOST == :win32)
		# bug in windows command line parser causes commands on the following format to fail:
		# \directory\subdir/executable
		# to work around, we substitute all \ for /.
		return cmd.gsub('\\', '/')
	end
	return cmd
end

# This should always be used instead of the ENV.
def mosyncdir
	m = ENV['MOSYNCDIR']
	raise "Environment variable MOSYNCDIR is not set" if(!m)
	return switchPathSlashes(m)
end

require "#{File.dirname(__FILE__)}/host.rb"
require "#{File.dirname(__FILE__)}/error.rb"
