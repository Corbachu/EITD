//----------------------------------------------------------------------------
//  EDGE IN THE DARK
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2021  The EDGE Team.
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------
//
//  Based on the DOOM source code, released by Id Software under the
//  following copyright:
//
//    Copyright (C) 1993-1996 by id Software, Inc.
//
//  Based on the FITD source code, under the GPLv2, (C) FITD AUTHORS
//
//----------------------------------------------------------------------------

#ifndef _COMMON_H_
#define _COMMON_H_

#include "epi/epi.h"
#include "system/i_system.h"

#ifdef MACOSX
#define UNIX
#endif

#ifdef WIN32
#include "system/win32/w32_sysinc.h"
#endif

#define NUM_MAX_CAMERA_IN_ROOM 20

#define HAS_YM3812 1

#ifndef USE_GL
#define USE_GL
#endif

#ifdef USE_GL
#   if defined(__APPLE__)
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#   else
#       include <GL/gl.h>      // Header File For The OpenGL32 Library
#       include <GL/glu.h>     // Header File For The GLu32 Library
#   endif
#endif

#ifdef HAVE_CONFIG_H
#undef HAVE_CONFIG_H
#include "common/endian.h"
#define HAVE_CONFIG_H
#else
#include "epi/types.h"
#include "epi/endianess.h"
#endif

#include "epi/asserts.h"

//////////////// GAME SPECIFIC DEFINES

//#define NUM_MAX_OBJ         300
#define NUM_MAX_ACTOR       50
#define NUM_MAX_TEXT        40
#define NUM_MAX_MESSAGE     5
#define INVENTORY_SIZE      30


//////////////////

#include "cvars.h"

//////////////////
#define TYPE_MASK 0x1D1

#define ANIM_ONCE             0
#define ANIM_REPEAT           1
#define ANIM_UNINTERRUPTABLE  2
#define ANIM_RESET            4

// temp triangulation stuff
//#include <bool.h>
//#include <geometry.h>
int triangulate_polygon(int ncontours, int cntr[], double(*vertices)[2], int (*triangles)[3]);
#include "room.h"
#include "vars.h"
#include "main.h"
#include "file_access.h"
#include "screen.h"
#include "video_mode.h"
#include "pak.h"
#include "unpack.h"
#include "e_tatou.h"
#include "thread_code.h"
#include "renderer.h"
#include "input.h"
#include "cos_table.h"
#include "hqr.h"
#include "timer.h"
#include "font.h"
#include "aitd_box.h"
#include "save.h"
#include "anim.h"
#include "anim_action.h"
#include "actor_list.h"
#include "inventory.h"
#include "startup_menu.h"
#include "system_menu.h"
#include "floor.h"
#include "object.h"
#include "zv.h"
#include "music.h"


// debugger
#ifdef INTERNAL_DEBUGGER
#include "debugger.h"
#endif

// scripting
#include "track.h"
#include "life.h"
#include "life_2.h"
#include "eval_var.h"

//// C++
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#ifdef WIN32
#include <search.h>
#endif



// Disable byte-swapping for now.
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif

FORCEINLINE uint16 READ_BE_U16(void *ptr) 
{
#ifdef BIG_ENDIAN
	return *(u16 *)ptr;
#else
	return (((uint8 *)ptr)[1] << 8) | ((uint8 *)ptr)[0];
#endif
}

FORCEINLINE int16 READ_BE_S16(void *ptr) 
{
	return (int16)READ_BE_S16(ptr);
}

FORCEINLINE uint32 READ_BE_U32(void *ptr) 
{
#ifdef BIG_ENDIAN
	return *(u32 *)ptr;
#else
	return (((uint8 *)ptr)[3] << 24) | (((uint8 *)ptr)[2] << 16) | (((uint8 *)ptr)[1] << 8) | ((uint8 *)ptr)[0];
#endif
}

#endif
