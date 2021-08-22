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

#include "i_sdlinc.h"

#include "common.h"

namespace Fitd 
{

int musicThread(void *dummy);

int threadTimer(void *test) 
{ // AITD time is 50 frames per seconds ie,  a frame every 20 miliseconds
	int temp = SDL_GetTicks();
	while(1) 
	{
		SDL_Delay(2); // granularity restriction
		if(SDL_GetTicks() - temp >= 15) 
		{
			// timeGlobal++;
			temp = SDL_GetTicks();
		}
	}
	return (0);
}

void startThreadTimer() 
{

}

} // end of namespace Fitd
