#!/usr/bin/ruby

require File.expand_path('../../rules/native_mosync.rb')

MoSyncExe.new do
	@SOURCES = ["."]
	@NAME = "PanicDoc"
end

Works.run
