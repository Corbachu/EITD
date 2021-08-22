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

//#include "common/textconsole.h" w32_system
#include "common.h"
#include "fitd.h"
#include "resource.h"
#include "textes.h"

namespace Fitd {

Textes *g_textes = NULL;

// 250
#define NUM_MAX_TEXT_ENTRY  1000

const char *languageNameTable[] = {
	"ENGLISH",
	"FRANCAIS",
	"ITALIANO",	
	"ESPAGNOL",
	"DEUTSCH",
	NULL,
};

Textes::Textes() 
{
	int currentIndex;
	char *currentPosInTextes;
	int textCounter;
	int stringIndex;
	char *stringPtr;
	int textLength;

	_tabTextes = new textEntryStruct[NUM_MAX_TEXT_ENTRY]; // 2000 = 250 * 8

	//SYS_ASSERT_PTR(_tabTextes);

	if(!_tabTextes) 
	{
		theEnd(1, "TabTextes");
	}

	// setup languageNameString
	if(g_fitd->getGameType() == GType_AITD3) 
	{
		strcpy(languageNameString, "TEXTES");
	} else 
	{
		int i = 0;

		while(languageNameTable[i]) {
			char tempString[20];

			strcpy(tempString, languageNameTable[i]);
			strcat(tempString, ".PAK");

			if(g_resourceLoader->getFileExists(tempString)) {
				strcpy(languageNameString, languageNameTable[i]);
				break;
			}

			i++;
		}
	}

	if(!languageNameString[0]) {
		I_Error("Unable to detect language file..\n");
	}

	char *systemTextes = g_resourceLoader->loadPakSafe(languageNameString, 0); // todo: use real language name
	textLength = getPakSize(languageNameString, 0);

	for(currentIndex = 0; currentIndex < NUM_MAX_TEXT_ENTRY; currentIndex++) {
		_tabTextes[currentIndex].index = -1;
		_tabTextes[currentIndex].textPtr = NULL;
		_tabTextes[currentIndex].width = 0;
	}

	currentPosInTextes = systemTextes;

	textCounter = 0;

	while(currentPosInTextes < systemTextes + textLength) {
		currentIndex = *(currentPosInTextes++);

		if(currentIndex == 26)
			break;

		if(currentIndex == '@') { // start of string marker
			stringIndex = 0;

			while((currentIndex = *(currentPosInTextes++)) >= '0' && currentIndex <= '9') { // parse string number
				stringIndex = stringIndex * 10 + currentIndex - 48;
			}

			if(currentIndex == ':') { // start of string
				stringPtr = currentPosInTextes;

				do {
					currentPosInTextes ++;
				} while((unsigned char)*(currentPosInTextes - 1) >= ' '); // detect the end of the string

				*(currentPosInTextes - 1) = 0; // add the end of string

				_tabTextes[textCounter].index = stringIndex;
				_tabTextes[textCounter].textPtr = stringPtr;
				_tabTextes[textCounter].width = computeStringWidth(stringPtr);

				textCounter++;
			}

			if(currentIndex == 26) {
				return;
			}
		}
	}
}

textEntryStruct *Textes::getTextFromIdx(int index) {
	for(int currentIndex = 0; currentIndex < NUM_MAX_TEXT_ENTRY; currentIndex++) {
		if(_tabTextes[currentIndex].index == index) {
			return(&_tabTextes[currentIndex]);
		}
	}

	return(NULL);
}

} // end of namespace Fitd
