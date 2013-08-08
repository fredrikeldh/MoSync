#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	@SOURCES = ['.', 'GraphAPI', 'GraphAPI/GetConnections',
		'GraphAPI/GetFacebookObjects', 'GraphAPI/GetFacebookObjects/FacebookObjects',
		'GraphAPI/Publish', 'HTTP', 'JSON_lib']
	@EXTRA_INCLUDES = ['.']
	@SPECIFIC_CFLAGS = {
		'FacebookPublisher2.cpp' => ' -Wno-vla',
	}

	@HEADER_DIRS = [
		'GraphAPI/GetFacebookObjects/FacebookObjects',
		'GraphAPI/GetFacebookObjects',
		'GraphAPI/GetConnections',
		'GraphAPI/Publish',
		'GraphAPI',
		'JSON_lib',
		'HTTP',
		'.',
	]
	@HEADER_INSTALLDIR = 'Facebook'

	@NAME = 'Facebook'
end

Works.run
