#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

MoSyncLib.new do
	entities = FileTask.new('entities.c') do
		@prerequisites << FileTask.new('entities.txt')
	end
	entities.instance_eval do
		def fileExecute
			tfn = 'build/_temp.c'
			sh "gperf -tCE --language=ANSI-C --lookup-function-name=entity_lookup entities.txt " +
				"| #{sed('s/#line/\\/\\/line/')} > #{tfn}"
			if(File.size(tfn) == 0)
				error "GPERF failed!"
			end
			FileUtils.mv(tfn, @NAME)
		end
	end

	@SOURCES = ["."]
	@IGNORED_FILES = ['entities.c']
	@IGNORED_HEADERS = ['entities.h']
	@EXTRA_SOURCETASKS = [entities]
	@SPECIFIC_CFLAGS = {"MTXml.cpp" => " -Wno-unreachable-code",
		"entities.c" => " -Wno-extra",
	}
	@HEADER_INSTALLDIR = "MTXml"
	@NAME = "mtxml"
end

Works.run
