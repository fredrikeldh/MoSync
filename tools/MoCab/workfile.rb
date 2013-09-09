#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

ExeWork.new do
	@SOURCES = ["lcab"]
	@NAME = "lcab"
	@TARGETDIR = "."
	@INSTALLDIR = mosyncdir + '/bin'
end

Works.run
