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

#ifndef MSTYPEINFO_H
#define MSTYPEINFO_H

#if 0
#include <base/Syscall.h>
#else
#define memset __memset
#define memcpy __memcpy
#define strcpy __strcpy
#define strcmp __strcmp
#ifdef __GNUC__
#define SYSCALL(type, nargs) type nargs __attribute__((visibility("default")))
#else
#define SYSCALL(type, nargs) type nargs
#endif
#define __IPHONE__
#include <helpers/cpp_defs.h>
#include <helpers/cpp_maapi.h>
#endif

#include <stdint.h>
#include <math.h>

#undef SYSCALL

#define SYSCALL_IMPL(x) ::x
//#define SYSCALL_IMPL(x) x

#define RINT(xx) 		*(int*)(mem_ds + (xx))
#define RSHORT(xx) 		*(unsigned short*)(mem_ds + (xx))
#define RBYTE(xx) 		*(mem_ds + (xx))

#define WINT(xx,yy)		RINT(xx) = yy
#define WSHORT(xx,yy)	RSHORT(xx) = yy
#define WBYTE(xx,yy)	RBYTE(xx) = yy

//#define SXSHORT(xx) (int)((short)(xx))
//#define SXBYTE(xx) (int)((char)(xx))
//#define SXSHORT(xx) ((((xx) & 0x8000) == 0) ? ((xx) & 0xFFFF) : ((xx) | ~0xFFFF))
//#define SXBYTE(xx) ((((xx) & 0x80) == 0) ? ((xx) & 0xFF) : ((xx) | ~0xFF))

#define SYSCALL(name)	wrap_##name

union FREG {
	double d;
	uint64_t ll;
	int i[2];
};

void MoSyncDiv0();

extern int sp;

extern unsigned char* mem_ds;

#include "syscall_static_cpp.h"

#undef SYSCALL
#define SYSCALL(name)	wrap##name

#define zr 0

void entryPoint();

unsigned char* CppInitReadData(const char* file, int fileSize, int mallocSize);

//#define byte char

#endif	//MSTYPEINFO_H
