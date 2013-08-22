#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@LST_PREREQS = [BundleTask.new('./Resources/LocalFiles.bin', './LocalFiles')]
	@LSTFILES = ['Resources/Resources.lst']
	@LIBRARIES = ['mautil', 'Wormhole', 'mafs', 'nativeui', 'yajl', 'Notification']
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'WebViewLoveSMS'
end

Works.run
