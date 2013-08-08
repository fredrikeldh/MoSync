#!/usr/bin/ruby

require File.expand_path('../../rules/cExe.rb')
require File.expand_path('../../rules/mosync_util.rb')

ExeWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["../../intlibs"]
	@LOCAL_LIBS = ["idl-common"]
	@NAME = "mx-config"
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
