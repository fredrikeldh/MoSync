/* Copyright (C) 2010 MoSync AB

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

#include "IOCtl.h"
#include <helpers/cpp_defs.h>

namespace Base
{
	int _maFrameBufferGetInfo(MAFrameBufferInfo *info)
	{
		int size = maGetScrSize();
		int width = (size&0xffff0000) >> 16;
		int height = size&0x0000ffff;

		info->bitsPerPixel = 32;
		info->bytesPerPixel = 4;
		info->redMask = 0x000000ff;
		info->greenMask = 0x0000ff00;
		info->blueMask = 0x00ff0000;


		info->width = width;
		info->height = height;
		info->pitch = info->width*4;

		info->sizeInBytes = info->pitch * info->height;

		info->redShift = 0;
		info->greenShift = 8;
		info->blueShift = 16;

		info->redBits = 8;
		info->greenBits = 8;
		info->blueBits = 8;

		info->supportsGfxSyscalls = 0;

		return 1;
	}


	int _maFrameBufferInit(void *data, int memStart, JNIEnv* jNIEnv, jobject jThis)
	{
		int rdata = (int)data - memStart;

		//char* b = (char*)malloc(200);
		//sprintf(b,"Framebuffer data: %i", rdata);
		//__android_log_write(ANDROID_LOG_INFO,"JNI",b);
		//free(b);

		jclass cls = jNIEnv->GetObjectClass(jThis);
		jmethodID methodID = jNIEnv->GetMethodID(cls, "_enableFramebuffer", "(I)V");

		int returnValue = -1;

		if (methodID != 0)
		{
			returnValue = 1;
			jNIEnv->CallVoidMethod(jThis, methodID, rdata);
		}
		jNIEnv->DeleteLocalRef(cls);

		return returnValue;
	}

	int _maFrameBufferClose(JNIEnv* jNIEnv, jobject jThis)
	{
		jclass cls = jNIEnv->GetObjectClass(jThis);
		jmethodID methodID = jNIEnv->GetMethodID(cls, "_disableFramebuffer", "()V");

		int returnValue = -1;
		if (methodID != 0)
		{
			returnValue = 1;
			jNIEnv->CallVoidMethod(jThis, methodID);
		}

		jNIEnv->DeleteLocalRef(cls);

		return returnValue;
	}
}
