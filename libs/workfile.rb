#!/usr/bin/ruby

require File.expand_path('../rules/subdir.rb')
require File.expand_path('../rules/mosync.rb')

target :pipe do
	Works.invoke_subdirs(SUBDIRS, 'pipe')
end

target :native do
	Works.invoke_subdirs(SUBDIRS, 'native')
end

target :arm do
	Works.invoke_subdirs(SUBDIRS, 'arm')
end

target :default do
	Works.invoke_subdirs(SUBDIRS)
end

target :clean do
	Works.invoke_subdirs(SUBDIRS, 'clean')
end

if(USE_NEWLIB)
	stdlibs = ["newlib", "stlport"]
else
	stdlibs = ["MAStd"]
end

SUBDIRS = stdlibs + ["MAUtil", "MTXml", "MAUI", "MAUI-revamp", "MATest", "MAP",
	"Testify", "MAFS", "yajl", "Ads", "Facebook", "NativeUI", "Notification", "Wormhole", "Purchase"]

if(HAVE_LIBC)
	SUBDIRS << "MoGraph"
end

if(MOSYNC_NATIVE)
	CopyDirTask.new(mosyncdir + "/include/glm", 'glm')
end

Works.run
