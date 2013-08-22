#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

MoSyncExe.new do
	@SOURCES = ['.', 'UI', 'Logic', 'Database']
	@SPECIFIC_CFLAGS = {
		'DatabaseManager.cpp' => ' -Wno-vla',
	}
	@LIBRARIES = ['mautil', 'nativeui', 'Purchase']
	@EXTRA_LINKFLAGS = standardMemorySettings(11)
	@NAME = 'PurchaseExample'
end

Works.run
