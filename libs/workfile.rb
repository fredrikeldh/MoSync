#!/usr/bin/ruby

require File.expand_path('../rules/task.rb')
require File.expand_path('../rules/targets.rb')

target :pipe do
	Work.invoke_subdirs(SUBDIRS, 'pipe')
end

target :native do
	Work.invoke_subdirs(SUBDIRS, 'native')
end

target :arm do
	Work.invoke_subdirs(SUBDIRS, 'arm')
end

target :default do
	if(USE_ARM)
		Work.invoke_subdirs(SUBDIRS, 'arm')
	else
		Work.invoke_subdirs(SUBDIRS, 'pipe')
	end
end

target :clean do
	Work.invoke_subdirs(SUBDIRS, 'clean')
end

Targets.setup

if(USE_NEWLIB)
	stdlibs = ["newlib", "stlport"]
else
	stdlibs = ["MAStd"]
end

SUBDIRS = stdlibs + ["MAUtil", "MTXml", "MAUI", "MAUI-revamp", "MATest", "MAP",
	"Testify", "stlport", "MAFS", "yajl", "Ads", "Facebook", "NativeUI", "Notification", "ResCompiler", "Wormhole",
]

if(USE_NEWLIB)
	SUBDIRS << 'SDL'
end

Targets.invoke
