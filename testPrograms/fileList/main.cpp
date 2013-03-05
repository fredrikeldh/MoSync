/* Copyright (C) 2009 Mobile Sorcery AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#include <ma.h>
#include <maassert.h>
#include <conprint.h>
#include <MAUtil/String.h>

#if 0	//log to file
#define LOG(str) lprintfln(str.c_str())
#define LPRINTFLN lprintfln
#else
#if 1	//log to screen
#define LOG(str) puts((str + "\n").c_str())
#define LPRINTFLN(fmt, a...) printf(fmt "\n", a)
#else	//don't log anything (useful for timing the list operation)
#define LOG(str)
#define LPRINTFLN(fmt, a...)
#endif
#endif

static void checkEvents() {
	MAEvent event;
	while(maGetEvent(&event)) {
		if(event.type == EVENT_TYPE_CLOSE ||
			(event.type == EVENT_TYPE_KEY_PRESSED && event.key == MAK_0))
		{
			maExit(1);
		}
	}
}

static char buffer[256];
static int sCount = 0;

static bool dumpFileList(const char* path) {
	MAHandle list = maFileListStart(path, "*", 0);
	if(list < 0) {
		LPRINTFLN("FLS error %i", list);
		return false;
	}
	bool empty = true;
	while(maFileListNext(list, buffer, sizeof(buffer)) > 0) {
		checkEvents();
		MAUtil::String p2(path);
		p2 += buffer;
		LOG(p2);
		sCount++;
		if(sCount % 100 == 0) {
			printf("%i\n", sCount);
		}
		if(p2[p2.size()-1] == '/')
			dumpFileList(p2.c_str());
		empty = false;
	}
	maFileListClose(list);
	//printf("%s: %s\n", empty ? "Empty" : "Done", path);
	return !empty;
}

extern "C" int MAMain() GCCATTRIB(noreturn);
extern "C" int MAMain() {
	InitConsole();
	gConsoleLogging = 1;
	printf("dumping File List...\n");
	int startTime = maGetMilliSecondCount();
	dumpFileList("");
	int endTime = maGetMilliSecondCount();
	printf("Done in %i ms\n", endTime - startTime);
	printf("%i files total\n", sCount);

	FREEZE;
}
