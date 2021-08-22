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

#include "common.h"

namespace Fitd 
{

volatile uint32 timeGlobal;
bool timerSaved = false;
unsigned int timerSavedValue = 0;

void freezeTime(void) 
{
	if(!timerSaved) 
	{
		timerSavedValue = timeGlobal;
		timerSaved = true;
	}
}

void unfreezeTime(void) 
{
	if(timerSaved) 
	{
		timeGlobal = timerSavedValue;
		timerSaved = false;
	}
}

} // end of namespace Fitd
