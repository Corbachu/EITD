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

#ifdef INTERNAL_DEBUGGER

typedef struct 
{
	uint32 X;
	uint32 Y;
	uint8 name[40];
	uint8 *returnVar;
} debugger_buttonEntry;

bool debuggerVar_debugMenuDisplayed = false;

////// debug var used in engine
bool debuggerVar_drawModelZv = false;
bool debuggerVar_drawCameraCoverZone = false;
bool debuggerVar_noHardClip = false;
bool debuggerVar_topCamera = false;
long int debufferVar_topCameraZoom = -4000;

bool debuggerVar_useBlackBG = false;
///////////////////////////////

uint8 debugger_mainDebugButtonVar_toggleDrawModelZv = 0;
uint8 debugger_mainDebugButtonVar_toggleDrawCameraCoverZone = 0;

const debugger_buttonEntry debugger_mainDebugButtonTable[] = 
{
	{0, 0, "Draw model ZV", &debugger_mainDebugButtonVar_toggleDrawModelZv},
	{0, 8, "Draw camera cover zone", &debugger_mainDebugButtonVar_toggleDrawCameraCoverZone},
	{0, 0, "", NULL},
};

const debugger_buttonEntry *debugger_currentMenu;

void debugger_processDebugMenu(void) 
{
}

void debugger_enterMainDebug(void) 
{
	debuggerVar_debugMenuDisplayed = true;

	debugger_currentMenu = debugger_mainDebugButtonTable;
}

void debugger_drawMenuEntry(const debugger_buttonEntry *menuEntry) 
{
	g_driver->drawDebugText(menuEntry->X, menuEntry->Y, menuEntry->name);
}

void debugger_draw(void) 
{
	if(debuggerVar_debugMenuDisplayed) 
	{
		uint32 numEntryInMenu = 0;
		uint32 currentMenuEntry;

		while(debugger_currentMenu[numEntryInMenu].name[0] != 0) 
		{
			numEntryInMenu++;
		}

		for(currentMenuEntry = 0; currentMenuEntry < numEntryInMenu; currentMenuEntry++) 
		{
			debugger_drawMenuEntry(&debugger_currentMenu[currentMenuEntry]);
		}
	}
}

#endif

} // end of namespace Fitd
