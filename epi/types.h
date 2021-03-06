//------------------------------------------------------------------------
//  EDGE Type definitions
//------------------------------------------------------------------------
//
//  Copyright (c) 2003-2008  The EDGE Team.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  (LGPL) as published by the Free Software Foundation.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//------------------------------------------------------------------------

#ifndef __EPI_TYPE_H__
#define __EPI_TYPE_H__

// basic types

typedef signed char  s8_t;
typedef signed short s16_t;
typedef signed int   s32_t;
typedef long int l32_t;
 
typedef unsigned char  u8_t; //BYTE
typedef unsigned short u16_t; //WORD
typedef unsigned int   u32_t;
typedef unsigned long long int u64_t;
//typedef unsigned int   u64_t;

typedef u8_t byte;

typedef u8_t uint8;
typedef s32_t int32;
typedef s16_t int16;
typedef u16_t uint16;
typedef u32_t uint32;

#ifdef __GNUC__
typedef long long i64_t;
#else
typedef __int64 i64_t;
#endif

#endif  /*__EPI_TYPE_H__*/

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
