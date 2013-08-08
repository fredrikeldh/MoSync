#!/usr/bin/ruby

require File.expand_path('../../rules/cExe.rb')
require File.expand_path('../../rules/mosync_util.rb')

ExeWork.new do
	@SOURCES = [".", "deflate/deflate", "deflate/host"]
	@EXTRA_INCLUDES = ["../../intlibs", "deflate/inc"]
	@NAME = "e32hack"

	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
