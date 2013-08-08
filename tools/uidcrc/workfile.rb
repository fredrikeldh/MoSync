#!/usr/bin/ruby

require File.expand_path('../../rules/cExe.rb')
require File.expand_path('../../rules/mosync_util.rb')

ExeWork.new do
	@SOURCES = ["."]
	@EXTRA_INCLUDES = ["./inc"]
	@NAME = "uidcrc"
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
