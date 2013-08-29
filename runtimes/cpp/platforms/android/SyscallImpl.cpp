/* Copyright (C) 2012 MoSync AB

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

#include "config_platform.h"
#include "Syscall.h"
#include "Core.h"

#include "IOCtl.h"
#include "EventQueue.h"

// Not used, instead class EventQueue is used.
//#include <helpers/fifo.h>

#include <jni.h>
#include <GLES/gl.h>

// we only expose the GL_OES_FRAMEBUFFER_OBJECT extension for now.
#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>

#ifndef _android_1
#include <GLES2/gl2.h>
#endif

#include "helpers/CPP_IX_AUDIOBUFFER.h"
#include "helpers/CPP_IX_OPENGL_ES.h"
#include "helpers/CPP_IX_OPENGL_ES_MA.h"
#include "helpers/CPP_IX_GL1.h"
#include "helpers/CPP_IX_GL2.h"
#include "helpers/CPP_IX_GL_OES_FRAMEBUFFER_OBJECT.h"
#include "helpers/CPP_IX_PIM.h"
#include "helpers/CPP_IX_CELLID.h"
#include "jniIOCtl.h"
#include "JNIStrConv.h"

#define ERROR_EXIT { MoSyncErrorExit(-1); }

#include "syslog.h"

namespace Base
{
	Syscall* gSyscall;

	/**
	* TODO: Remove this Global Reference to JNIEnv
	*/
	JNIEnv* mJNIEnv = 0;
	jobject mJThis;

	/**
	* A Reference to the Java Virtual Machine
	*/
	JavaVM * mJavaVM;

	int mReloadHandle = 0;
	bool mIsReloading = false;

	static ResourceArray gResourceArray;
	static EventQueue gEventFifo;
	//static CircularFifo<MAEvent, EVENT_BUFFER_SIZE> gEventFifo;

	int gClipLeft = 0;
	int gClipTop = 0;
	int gClipWidth = 0;
	int gClipHeight = 0;

	/**
	 * Flag to keep track of whether the initial clip rect has
	 * been set of not. Used in maGetClipRect and maSetClipRect.
	 */
	int gClipRectIsSet = 0;

	MAHandle gDrawTargetHandle = HANDLE_SCREEN;


	/**
	* Syscall constructor
	*
	* Sets the global syscall to use this newly consturcted object.
	* If the event queue isn't empty, we will clear it since this is
	* only called when an application has restarted
	*/
	Syscall::Syscall()
	{
		gSyscall = this;
		// mIsLooked = false;
		// mGotLockedEvent = false;
		init();

		// Make sure the event queue is empty.
		gEventFifo.clear();
	}

	/**
	* Returns the JNI Environment variable.
	*
	* @ return The JNI Environment
	*/
	JNIEnv* Syscall::getJNIEnvironment()
	{
		JNIEnv* env = NULL;
		if (mJavaVM->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
		{
			return NULL;
		}
		return env;
	}

	/**
	* Returns the Java Virtual Machine instance.
	*
	* @ return The JNI Environment
	*/
	JavaVM* Syscall::getJavaVM()
	{
		return mJavaVM;
	}

	/**
	* sets the current JavaVM, Used for accessing JNI environmental variables
	*/
	void Syscall::setJavaVM(JavaVM* jvm)
	{
		mJavaVM = jvm;
	}

	/**
	* Returns a reference to the JNI Object which initiated the execution of this native code
	*
	* @return A reference the JNI  Objec
	*/
	jobject Syscall::getJNIThis()
	{
		return mJThis;
	}

	/**
	*
	*
	*/
	bool Syscall::loadImage(int resourceIndex, int pos, int length, int binaryResourceHandle)
	{
		SYSLOG("loadImage");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "loadImage", "(IIII)Z");
		if (methodID == 0) ERROR_EXIT;
		bool retVal = mJNIEnv->CallBooleanMethod(mJThis, methodID, resourceIndex, pos, length, binaryResourceHandle);

		mJNIEnv->DeleteLocalRef(cls);
		return retVal;
	}

	Image* Syscall::loadSprite(void* surface, ushort left, ushort top, ushort width, ushort height, ushort cx, ushort cy)
	{
		SYSLOG("loadSprite - NOT IMPLEMENTED");

		return NULL;
	}

	char* Syscall::loadBinary(int resourceIndex, int size)
	{
		SYSLOG("loadBinary");
		//get current thread's JNIEnvironmental variable
		JNIEnv * env = getJNIEnvironment();

		// Debug print.
		/*
		char* b = (char*)malloc(200);
		sprintf(b, "loadBinary index:%d size:%d", resourceIndex, size);
		__android_log_write(ANDROID_LOG_INFO, "MoSync Syscall", b);
		free(b);
		*/

		char* buffer = (char*)malloc(size);
		if(buffer == NULL) return NULL;

		jobject byteBuffer = env->NewDirectByteBuffer((void*)buffer, size);
		if(byteBuffer == NULL) return NULL;

		jclass cls = env->GetObjectClass(mJThis);
		jmethodID methodID = env->GetMethodID(cls, "loadBinary", "(ILjava/nio/ByteBuffer;)Z");
		if (methodID == 0) return NULL;

		jboolean ret = env->CallBooleanMethod(mJThis, methodID, resourceIndex, byteBuffer);

		env->DeleteLocalRef(cls);
		env->DeleteLocalRef(byteBuffer);

		if(ret == false)
		{
			free(buffer);
			return NULL;
		}

		return buffer;
	}

	int Syscall::loadBinaryStore(int resourceIndex, int size)
	{
		char* b = (char*)malloc(200);
		sprintf(b, "loadBinaryStore index:%d size:%d", resourceIndex, size);
		//__android_log_write(ANDROID_LOG_INFO, "MoSync Syscall", b);
		free(b);
		return maCreateData(resourceIndex, size);
	}

	void Syscall::loadUBinary(int resourceIndex, int offset, int size)
	{
		SYSLOG("loadUBinary");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "loadUBinary", "(III)V");
		if (methodID == 0) return;
		mJNIEnv->CallVoidMethod(mJThis, methodID, resourceIndex, offset, size);

		mJNIEnv->DeleteLocalRef(cls);
	}

	bool Syscall::destroyBinaryResource(int resourceIndex)
	{
		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "destroyBinary", "(I)Ljava/nio/ByteBuffer;");
		if (methodID == 0) return false;

		jobject jo = mJNIEnv->CallObjectMethod(mJThis, methodID, resourceIndex);
		bool destroyed = false;
		if(jo != NULL)
		{
			char* buffer = (char*)mJNIEnv->GetDirectBufferAddress(jo);
			free(buffer);
			destroyed = true;
		}

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jo);

		return destroyed;
	}

	void Syscall::destroyResource(int resourceIndex)
	{
		SYSLOG("destroyResource");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "destroyResource", "(I)V");
		if (methodID == 0) return;
		mJNIEnv->CallVoidMethod(mJThis, methodID, resourceIndex);

		mJNIEnv->DeleteLocalRef(cls);
	}

	/**
	* Calls the Java function 'storeIfBinaryAudioresource'.
	* If this resource is an audio resource, with a correct mime header,
	* this file will be saved to the memoory.
	* This is because Android can only play commpressed audio formats
	* with a file descriptor and not from a buffer or array in memory.
	*
	* @param resourceIndex		The resource index of the resource which shall be checked.
	*
	*/
	void Syscall::checkAndStoreAudioResource(int resourceIndex)
	{
		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "storeIfBinaryAudioResource", "(I)V");
		if (methodID != 0)
			mJNIEnv->CallVoidMethod(mJThis, methodID, resourceIndex);

		mJNIEnv->DeleteLocalRef(cls);
	}

	void Syscall::platformDestruct()
	{
	}

	void Syscall::setJNIEnvironment(JNIEnv* je, jobject jthis)
	{
		SYSLOG("setJNIEnvironment");

		mJNIEnv = je;
		mJThis = jthis;

		testIOCtls(mJNIEnv, mJThis);
	}

	void Syscall::postEvent(MAEvent event)
	{
		SYSLOG("PostEvent");
		gEventFifo.put(event);
	}

	int Syscall::getEventQueueSize()
	{
		return gEventFifo.count();
	}

	SYSCALL(int,  maSetColor(int rgb))
	{
		SYSLOG("maSetColor");

		if(rgb<=0xffffff) rgb += 0xff000000;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSetColor", "(I)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, rgb);

		mJNIEnv->DeleteLocalRef(cls);
		return retval;
	}

	SYSCALL(void,  maSetClipRect(int left, int top, int width, int height))
	{
		SYSLOG("maSetClipRect");

		// Clip rect is now set.
		gClipRectIsSet = 1;

		gClipLeft = left;
		gClipTop = top;
		gClipWidth = width;
		gClipHeight = height;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSetClipRect", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, left, top, width, height);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maGetClipRect(MARect* rect))
	{
		SYSLOG("maGetClipRect");

		// If no clip rect is set, we set it to the screen size.
		if (!gClipRectIsSet)
		{
			MAExtent extent = maGetScrSize();
			gClipLeft = 0;
			gClipTop = 0;
			gClipWidth = EXTENT_X(extent);
			gClipHeight = EXTENT_Y(extent);

			// Clip rect is now set.
			gClipRectIsSet = 1;
		}

		gSyscall->ValidateMemRange(rect, sizeof(MARect));
		rect->left = gClipLeft;
		rect->top = gClipTop;
		rect->width = gClipWidth;
		rect->height = gClipHeight;
	}

	SYSCALL(void,  maPlot(int posX, int posY))
	{
		SYSLOG("maPlot");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maPlot", "(II)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, posX, posY);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maLine(int startX, int startY, int endX, int endY))
	{
		SYSLOG("maLine");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maLine", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, startX, startY, endX, endY);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maFillRect(int left, int top, int width, int height))
	{
		SYSLOG("maFillRect");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maFillRect", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, left, top, width, height);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maFillTriangleStrip(const MAPoint2d* points, int count))
	{
		SYSLOG("maFillTriangleStrip");

		int heapPoints = (int)points - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maFillTriangleStrip", "(II)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, heapPoints, count);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maFillTriangleFan(const MAPoint2d* points, int count))
	{
		SYSLOG("maFillTriangleFan");

		int heapPoints = (int)points - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maFillTriangleFan", "(II)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, heapPoints, count);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(MAExtent,  maGetTextSize(const char* str))
	{
		//SYSLOG("maGetTextSize");

		jstring jstr = mJNIEnv->NewStringUTF(str);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maGetTextSize", "(Ljava/lang/String;)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, jstr);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);

		return retval;
	}

	SYSCALL(MAExtent,  maGetTextSizeW(const wchar* str))
	{
		//SYSLOG("maGetTextSizeW");

		jstring jstr = WCHAR_TO_JCHAR(mJNIEnv, str);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maGetTextSizeW", "(Ljava/lang/String;)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, jstr);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);

		return retval;
	}

	SYSCALL(void,  maDrawText(int left, int top, const char* str))
	{
		//SYSLOG("maDrawText");

		jstring jstr = mJNIEnv->NewStringUTF(str);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maDrawText", "(IILjava/lang/String;)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, left, top, jstr);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);
	}

	SYSCALL(void,  maDrawTextW(int left, int top, const wchar* str))
	{
		//SYSLOG("maDrawTextW");

		jstring jstr = WCHAR_TO_JCHAR(mJNIEnv, str);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maDrawTextW", "(IILjava/lang/String;)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, left, top, jstr);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);
	}

	SYSCALL(void,  maUpdateScreen(void))
	{
		//SYSLOG("maUpdateScreen");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maUpdateScreen", "()V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);
	}

	/**
	* Reset backlight is not implemented on Android since it has nothing similar
	*/
	SYSCALL(void,  maResetBacklight(void))
	{
		//SYSLOG("maResetBacklight");
	}

	SYSCALL(MAExtent,  maGetScrSize(void))
	{
		SYSLOG("maGetScrSize");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maGetScrSize", "()I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(void,  maDrawImage(MAHandle image, int left, int top))
	{
		SYSLOG("maDrawImage");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maDrawImage", "(III)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, image, left, top);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maDrawRGB(const MAPoint2d* dstPoint, const void* src, const MARect* srcRect, int scanLength))
	{
		SYSLOG("maDrawRGB");

		int rsrc = (int)src - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "_maDrawRGB", "(IIIIIIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, dstPoint->x, dstPoint->y, rsrc, srcRect->left, srcRect->top, srcRect->width, srcRect->height, scanLength);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maDrawImageRegion(MAHandle image, const MARect* srcRect, const MAPoint2d* dstPoint, int transformMode))
	{
		SYSLOG("maDrawImageRegion");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "_maDrawImageRegion", "(IIIIIIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, image, srcRect->left, srcRect->top, srcRect->width, srcRect->height, dstPoint->x, dstPoint->y, transformMode);

		mJNIEnv->DeleteLocalRef(cls);

	}

	SYSCALL(MAExtent,  maGetImageSize(MAHandle image))
	{
		SYSLOG("maGetImageSize");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maGetImageSize", "(I)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, image);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(void,  maGetImageData(MAHandle image, void* dst, const MARect* srcRect, int scanlength))
	{
		SYSLOG("maGetImageData");

		if (srcRect->width > scanlength) maPanic(ERR_IMAGE_OOB, "maGetImageData, scanlenght < width");

		int rdst = (int)dst - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "_maGetImageData", "(IIIIIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, image, rdst, srcRect->left, srcRect->top, srcRect->width, srcRect->height, scanlength);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(MAHandle,  maSetDrawTarget(MAHandle image))
	{
		SYSLOG("maSetDrawTarget");

		MAHandle temp = gDrawTargetHandle;
		int currentDrawSurface;

		if(gDrawTargetHandle != HANDLE_SCREEN)
		{
			SYSCALL_THIS->resources.extract_RT_FLUX(gDrawTargetHandle);
			if(SYSCALL_THIS->resources.add_RT_IMAGE(
				gDrawTargetHandle, new int[1]) == RES_OUT_OF_MEMORY)
			{
				maPanic(ERR_RES_OOM, "maSetDrawTarget couldn't allocate drawtarget");
			}
			gDrawTargetHandle = HANDLE_SCREEN;
		}

		if(image == HANDLE_SCREEN)
		{
			currentDrawSurface = HANDLE_SCREEN;
		}
		else
		{
			currentDrawSurface = image;
			SYSCALL_THIS->resources.extract_RT_IMAGE(image);
			if(SYSCALL_THIS->resources.add_RT_FLUX(image, NULL) == RES_OUT_OF_MEMORY)
			{
				maPanic(ERR_RES_OOM, "maSetDrawTarget couldn't allocate drawtarget");
			}
		}
		gDrawTargetHandle = image;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSetDrawTarget", "(I)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, currentDrawSurface);

		mJNIEnv->DeleteLocalRef(cls);

		return temp;
	}

	SYSCALL(int,  maCreateImageFromData(MAHandle placeholder, MAHandle data, int offset, int size))
	{
		SYSLOG("maCreateImageFromData");

		if (SYSCALL_THIS->resources.add_RT_IMAGE(
			placeholder, new int[1]) == RES_OUT_OF_MEMORY)
		{
			return RES_OUT_OF_MEMORY;
		}

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maCreateImageFromData", "(IIII)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, placeholder, data, offset, size);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(int,  maCreateImageRaw(MAHandle placeholder, const void* src, MAExtent size, int alpha))
	{
		SYSLOG("maCreateImageRaw");

		int imgHeight = size&0xffff;
		int imgWidth = (size>>16)&0xffff;

		int imgSize = imgWidth * imgHeight * 4;

		// Malloc some memory to use when creating the image
		char* img = (char*)malloc(imgSize);
		if(img == NULL) return 0;

		jobject jBuffer = mJNIEnv->NewDirectByteBuffer((void*)img, imgSize);
		if(jBuffer == NULL) return 0;

		if(0==alpha)
		{
			char* srcImg = (char*)src;
			int j = 0;
			for(int i = 0 ; i < imgSize/4; i++)
			{
				(*(img+j)) = (*(srcImg+j+2));j++;
				(*(img+j)) = (*(srcImg+j));j++;
				(*(img+j)) = (*(srcImg+j-2));j++;
				(*(img+j)) = 255;j++;
			}
		}
		else
		{
			char* srcImg = (char*)src;
			int j = 0;
			for(int i = 0 ; i < imgSize/4; i++)
			{
				(*(img+j)) = (*(srcImg+j+2));j++;
				(*(img+j)) = (*(srcImg+j));j++;
				(*(img+j)) = (*(srcImg+j-2));j++;
				(*(img+j)) = (*(srcImg+j));j++;
			}
		}

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "_maCreateImageRaw", "(IIILjava/nio/ByteBuffer;)I");
		if (methodID == 0) ERROR_EXIT;
		int retVal = mJNIEnv->CallIntMethod(mJThis, methodID, placeholder, imgWidth, imgHeight, jBuffer);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jBuffer);

		free(img);

		SYSCALL_THIS->resources.add_RT_IMAGE(placeholder,  new int[1]);

		return retVal;
	}

	SYSCALL(int,  maCreateDrawableImage(MAHandle placeholder, int width, int height))
	{
		SYSLOG("maCreateDrawableImage");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maCreateDrawableImage", "(III)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, placeholder, width, height);

		mJNIEnv->DeleteLocalRef(cls);

		if(RES_OK == retval)
		{
			return SYSCALL_THIS->resources.add_RT_IMAGE(placeholder,  new int[1]);
		}

		return retval;
	}

	SYSCALL(MAHandle,  maOpenStore(const char* name, int flags))
	{
		SYSLOG("maOpenStore");

		jstring jname = mJNIEnv->NewStringUTF(name);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maOpenStore", "(Ljava/lang/String;I)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, jname, flags);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jname);

		return retval;
	}

	SYSCALL(int,  maWriteStore(MAHandle store, MAHandle data))
	{
		SYSLOG("maWriteStore");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maWriteStore", "(II)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, store, data);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(int,  maReadStore(MAHandle store, MAHandle placeholder))
	{
		SYSLOG("maReadStore");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "_maReadStore", "(II)I");
		if (methodID == 0) ERROR_EXIT;
		jint res = mJNIEnv->CallIntMethod(mJThis, methodID, store, placeholder);

		mJNIEnv->DeleteLocalRef(cls);

		return res;
	}

	SYSCALL(void,  maCloseStore(MAHandle store, int remove))
	{
		SYSLOG("maCloseStore");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maCloseStore", "(II)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, store, remove);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(MAHandle,  maConnect(const char* url))
	{
		SYSLOG("maConnect");

		jstring jstr = mJNIEnv->NewStringUTF(url);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnect", "(Ljava/lang/String;)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, jstr);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);

		return retval;
	}

	SYSCALL(void,  maConnClose(MAHandle conn))
	{
		SYSLOG("maConnClose");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnClose", "(I)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maConnRead(MAHandle conn, void* dst, int size))
	{
		SYSLOG("maConnRead");

		int rdst = (int)dst - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnRead", "(III)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, (jint)rdst, size);

		mJNIEnv->DeleteLocalRef(cls);

	}

	SYSCALL(void,  maConnWrite(MAHandle conn, const void* src, int size))
	{
		SYSLOG("maConnWrite");

		int rsrc = (int)src - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnWrite", "(III)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, (jint)rsrc, size);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void, maConnReadFrom(MAHandle conn, void* dst, int size, MAConnAddr* src))
	{
		SYSLOG("maConnReadFrom");

		int rdst = (int)dst - (int)gCore->mem_ds;
		int rsrc = (int)src - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnReadFrom", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, (jint)rdst, size, rsrc);

		mJNIEnv->DeleteLocalRef(cls);

	}

	SYSCALL(void, maConnWriteTo(MAHandle conn, const void* src, int size, const MAConnAddr* dst))
	{
		SYSLOG("maConnWriteTo");

		int rsrc = (int)src - (int)gCore->mem_ds;
		int rdst = (int)dst - (int)gCore->mem_ds;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnWriteTo", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, (jint)rsrc, size, rdst);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maConnReadToData(MAHandle conn, MAHandle data, int offset, int size))
	{
		SYSLOG("maConnReadToData");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnReadToData", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, data, offset, size);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maConnWriteFromData(MAHandle conn, MAHandle data, int offset, int size))
	{
		SYSLOG("maConnWriteFromData");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnWriteFromData", "(IIII)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, data, offset, size);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(int,  maConnGetAddr(MAHandle conn, MAConnAddr* addr))
	{
		SYSLOG("maConnGetAddr");

		int addrPointer = (int)addr - (int)gCore->mem_ds;
		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maConnGetAddr", "(II)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, conn, addrPointer);
		mJNIEnv->DeleteLocalRef(cls);
		return retval;
	}

	SYSCALL(MAHandle,  maHttpCreate(const char* url, int method))
	{
		SYSLOG("mahttpCreate");

		jstring jstr = mJNIEnv->NewStringUTF(url);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maHttpCreate", "(Ljava/lang/String;I)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, jstr, method);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);

		return retval;
	}

	SYSCALL(void,  maHttpSetRequestHeader(MAHandle conn, const char* key, const char* value))
	{
		SYSLOG("maHttpSetRequestHeader");

		jstring jstrKey = mJNIEnv->NewStringUTF(key);
		jstring jstrValue = mJNIEnv->NewStringUTF(value);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maHttpSetRequestHeader", "(ILjava/lang/String;Ljava/lang/String;)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn, jstrKey, jstrValue);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstrKey);
		mJNIEnv->DeleteLocalRef(jstrValue);
	}

	SYSCALL(int,  maHttpGetResponseHeader(MAHandle conn, const char* key, char* buffer, int bufSize))
	{
		SYSLOG("maHttpGetResponseHeader");

		int rbuffer = (int)buffer - (int)gCore->mem_ds;

		jstring jstr = mJNIEnv->NewStringUTF(key);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maHttpGetResponseHeader", "(ILjava/lang/String;II)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, conn, jstr, rbuffer, bufSize);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);

		return retval;
	}

	SYSCALL(void,  maHttpFinish(MAHandle conn))
	{
		SYSLOG("maHttpFinish");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maHttpFinish", "(I)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, conn);

		mJNIEnv->DeleteLocalRef(cls);
	}

	// TODO : Implement maLoadProgram

	int Syscall::getReloadHandle()
	{
		return mReloadHandle;
	}

	int Syscall::setReloadHandle(MAHandle handle)
	{
		mReloadHandle = handle;
	}

	bool Syscall::isReloading()
	{
		return mIsReloading;
	}

	void Syscall::setReloading(bool state)
	{
		mIsReloading = state;
	}

	void Syscall::resetSyscallState()
	{
		// Reset the state
		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "initSyscalls", "()V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		// Yield the runtime so that it can load the new program
		SYSCALL_THIS->VM_Yield();
	}

	SYSCALL(int, maLoadResource(MAHandle handle, MAHandle placeholder, int flag))
	{
		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maLoadResource", "(III)I");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallIntMethod(mJThis, methodID,
			handle, placeholder, flag);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(void,  maLoadProgram(MAHandle data, int reload))
	{
		SYSLOG("maLoadProgram");

		//__android_log_write(ANDROID_LOG_INFO, "MoSync Syscall", "@@@@ MA LOAD PROGRAM");

		mReloadHandle = data;

		if(0 == reload)
			mIsReloading = false;
		else
			mIsReloading = true;

		gSyscall->resetSyscallState();
	}

	// TODO : Implement maGetKeys

	SYSCALL(int,  maGetKeys(void))
	{
		SYSLOG("maGetKeys NOT IMPLEMENTED");
		return -1;
	}

	// Parameter event points to event object on the MoSync side.
	SYSCALL(int,  maGetEvent(MAEvent* event))
	{
		gSyscall->ValidateMemRange(event, sizeof(MAEvent));
#ifndef MOSYNC_NATIVE
		MYASSERT(((uint)event & 3) == 0, ERR_MEMORY_ALIGNMENT);	//alignment
#endif

		// Exit if event queue is empty.
		if (gEventFifo.count() == 0) return 0;

		// Copy runtime side event to MoSync side event.
		*event = gEventFifo.get();

		// Copy event data to memory on the MoSync side.
#ifndef MOSYNC_NATIVE
		#define HANDLE_CUSTOM_EVENT(eventType, dataType) if(event->type == eventType) { \
			memcpy(Core::GetCustomEventPointer(gCore), (void*)event->data, sizeof(dataType)); \
			delete (dataType*) event->data; \
			event->data = (int(Core::GetCustomEventPointer(gCore)) - int(gCore->mem_ds)); }
#else
#define HANDLE_CUSTOM_EVENT(eventType, dataType) if(event->type == eventType) { \
	memcpy(Core::GetCustomEventPointer(gCore), (void*)event->data, sizeof(dataType)); \
	delete (dataType*) event->data; \
	event->data = (int(Core::GetCustomEventPointer(gCore))); }
#endif
		// Macro CUSTOM_EVENTS is defined in runtimes/cpp/base/Syscall.h
		CUSTOM_EVENTS(HANDLE_CUSTOM_EVENT);

		return 1;
	}

	SYSCALL(void,  maWait(int timeout))
	{
		if(gEventFifo.count() != 0)
			return;

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maWait", "(I)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, timeout);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(longlong,  maTime(void))
	{
		SYSLOG("maTime");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maTime", "()I");
		if (methodID == 0) ERROR_EXIT;
		longlong retval = mJNIEnv->CallLongMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(longlong,  maLocalTime(void))
	{
		SYSLOG("maLocalTime");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maLocalTime", "()I");
		if (methodID == 0) ERROR_EXIT;
		longlong retval = mJNIEnv->CallLongMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(int,  maGetMilliSecondCount(void))
	{

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maGetMilliSecondCount", "()I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	// TODO: Implement maFreeObjectMemory

	SYSCALL(int,  maFreeObjectMemory(void))
	{
		SYSLOG("maFreeObjectMemory NOT IMPLEMENTED");
		return -1;
	}

	// TODO : Implement maTotalObjectMemory

	SYSCALL(int,  maTotalObjectMemory(void))
	{
		SYSLOG("maTotalObjectMemory NOT IMPLEMENTED");
		return -1;
	}

	// TODO : Implement maVibrate

	SYSCALL(int, maVibrate(int ms))
	{
		SYSLOG("maVibrate");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maVibrate", "(I)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, ms);
		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(void, maPanic(int result, const char* message))
	{
		SYSLOG("maPanic");

		Base::gSyscall->VM_Yield();

		jstring jstr = mJNIEnv->NewStringUTF(message);

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maPanic", "(ILjava/lang/String;)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, (jint)result, jstr);

		mJNIEnv->DeleteLocalRef(cls);
		mJNIEnv->DeleteLocalRef(jstr);
	}

	SYSCALL(int,  maSoundPlay(MAHandle soundResource, int offset, int size))
	{
		SYSLOG("maSoundPlay");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSoundPlay", "(III)I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID, soundResource, offset, size);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(void,  maSoundStop(void))
	{
		SYSLOG("maStopSound");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSoundStop", "()V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);
	}

	SYSCALL(int,  maSoundIsPlaying(void))
	{
		SYSLOG("maSoundIsPlaying");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSoundIsPlaying", "()I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(int,  maSoundGetVolume(void))
	{
		SYSLOG("maSoundGetVolume");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSoundGetVolume", "()I");
		if (methodID == 0) ERROR_EXIT;
		int retval = mJNIEnv->CallIntMethod(mJThis, methodID);

		mJNIEnv->DeleteLocalRef(cls);

		return retval;
	}

	SYSCALL(void,  maSoundSetVolume(int volume))
	{
		SYSLOG("maSoundSetVolume");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maSoundSetVolume", "(I)V");
		if (methodID == 0) ERROR_EXIT;
		mJNIEnv->CallVoidMethod(mJThis, methodID, volume);

		mJNIEnv->DeleteLocalRef(cls);
	}

	// TODO : Implement maInvokeExtension

	SYSCALL(longlong,  maExtensionFunctionInvoke(MAExtensionFunction function, int numargs, int args, int dummy))
	{
		SYSLOG("maExtensionFunctionInvoke");

		jclass cls = mJNIEnv->GetObjectClass(mJThis);
		jmethodID methodID = mJNIEnv->GetMethodID(cls, "maExtensionFunctionInvoke", "(I[II)I");
		if (methodID == 0) ERROR_EXIT;
		int memStart = (int)gCore->mem_ds;
		jintArray jargs = mJNIEnv->NewIntArray(numargs);
		jint* body = new jint[numargs];//mJNIEnv->GetIntArrayElements(jargs, 0);
		for (int i = 0; i < numargs; i++) {
			int ptr = ((int*) args)[i];
			body[i] = ptr;
		}
		mJNIEnv->SetIntArrayRegion(jargs, 0, numargs, body);
		int retVal = mJNIEnv->CallIntMethod(mJThis, methodID, function, jargs, memStart);
		//mJNIEnv->ReleaseIntArrayElements(jargs, body, 0);
		delete[] body;
		mJNIEnv->DeleteLocalRef(cls);

		return retVal;
	}

	// Temporary kludge to include the implementation of glString,
	// a better solution would be to get a .h generated and
	// then add gl.h.cpp to the list of files.
	#include <generated/gl.h.cpp>

#include "GLFixes.h"

	/**
	 * Utility function for displaying and catching pending
	 * exceptions.
	 */
	static void handlePendingExceptions(JNIEnv* env)
	{
		jthrowable exc;
		exc = env->ExceptionOccurred();
		if (exc)
		{
			//__android_log_write(
			//					ANDROID_LOG_INFO,
			//					"@@@ MoSync",
			//					"Found pending exception");
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}


	SYSCALL(longlong,  maIOCtl(int function, int a, int b, int c MA_IOCTL_ELLIPSIS))
	{
		SYSLOG("maIOCtl: %d", function);
		//__android_log_write(ANDROID_LOG_INFO, "MoSync Syscall", "maIOCtl");
		//handlePendingExceptions(mJNIEnv);

		switch(function)
		{
#ifndef MOSYNC_NATIVE
		maIOCtl_IX_OPENGL_ES_caselist
		maIOCtl_IX_GL1_caselist
#ifndef _android_1
		maIOCtl_IX_GL2_caselist
#endif
		maIOCtl_IX_GL_OES_FRAMEBUFFER_OBJECT_caselist
#endif

		case maIOCtl_maFrameBufferGetInfo:
			SYSLOG("maIOCtl_maFrameBufferGetInfo");
			return _maFrameBufferGetInfo(GVMRA(MAFrameBufferInfo));

		case maIOCtl_maFrameBufferInit:
			SYSLOG("maIOCtl_maFrameBufferInit");
			return _maFrameBufferInit(GVMRA(void*), (int)gCore->mem_ds, mJNIEnv, mJThis);

		case maIOCtl_maFrameBufferClose:
			SYSLOG("maIOCtl_maFrameBufferClose");
			return _maFrameBufferClose(mJNIEnv, mJThis);

		default:
			return jniIOCtl(mJNIEnv, mJThis, function, a, b, c MA_IOCTL_ARGS);
		} // End of switch
	}
}

bool reloadProgram()
{
	if(true == Base::gSyscall->isReloading())
	{
		Base::gSyscall->resetSyscallState();
		return true;
	}
	return false;
}

void MoSyncExit(int errorCode)
{
#ifdef MOSYNC_NATIVE
	exit(errorCode);
#else
	//__android_log_write(ANDROID_LOG_INFO, "MoSyncExit!", "Program has exited!");

	if(false == reloadProgram())
	{
		//__android_log_write(ANDROID_LOG_INFO, "MoSyncExit!", "nahh.. just die now");

		exit(errorCode);
	}
	else
	{
		//__android_log_write(ANDROID_LOG_INFO, "MoSyncExit!", "Should reload program");

		Base::gEventFifo.clear();

		SYSCALL_THIS->VM_Yield();
	}
#endif
}

void MoSyncErrorExit(int errorCode)
{
	if(false == reloadProgram())
	{
		char* b = (char*)malloc(200);
		sprintf(b, "MoSync error: %i", errorCode);
		//__android_log_write(ANDROID_LOG_INFO, "MoSyncErrorExit!", b);
		jstring jstr = Base::mJNIEnv->NewStringUTF(b);
		free(b);

		jclass cls = Base::mJNIEnv->GetObjectClass(Base::mJThis);
		jmethodID methodID = Base::mJNIEnv->GetMethodID(cls, "threadPanic", "(ILjava/lang/String;)V");
		if (methodID == 0) ERROR_EXIT;
		Base::mJNIEnv->CallVoidMethod(Base::mJThis, methodID, (jint)errorCode, jstr);

		Base::mJNIEnv->DeleteLocalRef(cls);
		Base::mJNIEnv->DeleteLocalRef(jstr);

		exit(errorCode);
	}

	Base::gEventFifo.clear();
	SYSCALL_THIS->VM_Yield();
}

namespace jni {
	void Panic(int i, const char* msg) {
		Base::maPanic(i, msg);
	}

	void* GetValidatedMemRange(int address, int size) {
		return Base::gSyscall->GetValidatedMemRange(address, size);
	}
	void ValidateMemRange(const void* ptr, int size) {
		Base::gSyscall->ValidateMemRange(ptr, size);
	}
	int ValidatedStrLen(const char* ptr) {
		return Base::gSyscall->ValidatedStrLen(ptr);
	}
	int GetValidatedStackValue(int offset) {
		return Base::gSyscall->GetValidatedStackValue(offset);
	}
	const char* GetValidatedStr(int address) {
		return Base::gSyscall->GetValidatedStr(address);
	}
	const wchar_t* GetValidatedWStr(int address) {
		return (wchar_t*)Base::gSyscall->GetValidatedWStr(address);
	}
	jstring wcharToJchar(JNIEnv* env, const wchar_t* src) {
		return WCHAR_TO_JCHAR(env, (wchar*)src);
	}
}
