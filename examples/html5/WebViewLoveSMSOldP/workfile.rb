#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')
require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_resources.rb')

MoSyncExe.new do
	@SOURCES = ['.']
	@PREREQUISITES = [BundleTask.new(self, './Resources/LocalFiles.bin', './LocalFiles')]
	@LSTFILES = ['Resources/Resources.lst']
	@LIBRARIES = ['mautil', 'wormhole', 'mafs', 'nativeui', 'yajl', 'Notification']
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'WebViewLoveSMSOldP'
end

Works.run
