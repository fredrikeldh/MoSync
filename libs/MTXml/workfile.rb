#!/usr/bin/ruby

require File.expand_path('../../rules/mosync_lib.rb')

class EntitiesTask < FileTask
	def initialize
		@prerequisites = [FileTask.new('entities.txt'), DirTask.new('build')]
		super('build/entities.c')
	end
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

MoSyncLib.new do
	@SOURCES = ["."]
	@SOURCE_TASKS = [EntitiesTask.new]
	@IGNORED_HEADERS = ['entities.h']
	@SPECIFIC_CFLAGS = {"MTXml.cpp" => " -Wno-unreachable-code",
		"entities.c" => " -Wno-extra -I.",
	}
	@HEADER_INSTALLDIR = "MTXml"
	@NAME = "mtxml"
end

Works.run
