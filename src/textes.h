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

#ifndef _TEXTES_
#define _TEXTES_
namespace Fitd {

struct textEntryStruct {
	int16 index;
	char *textPtr;
	int16 width;
};

class Textes {
public:
	Textes();
	textEntryStruct *getTextFromIdx(int index);
private:
	textEntryStruct *_tabTextes;
};

extern Textes *g_textes;

} // end of namespace Fitd

#endif
