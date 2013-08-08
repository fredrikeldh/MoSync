#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = [
		".",
		"Libs",
		"Libs/JSNativeUI",
		"Libs/Notification",
		"Libs/Orientation",
		"Libs/PhoneGap",
		"Libs/W3C",
		"Libs/Extensions",
	]
	@EXTRA_INCLUDES = ['.']

	@HEADER_DIRS = [
		'Libs',
		'Libs/JSNativeUI',
		'Libs/Notification',
		'Libs/Orientation',
		'Libs/PhoneGap',
		'Libs/W3C',
		'Libs/Extensions',
		'.',
	]
	@HEADER_INSTALLDIR = "Wormhole"

	@NAME = "Wormhole"
end

Works.run
