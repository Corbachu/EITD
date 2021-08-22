/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "osystem.h"
#include "common.h"

namespace Fitd 
{

void printString(int index, int textIndex, int selectedIndex) 
{
	int topPosition;

	topPosition = (((currentMenuBottom - currentMenuTop) / 2) + currentMenuTop) + (index << 4) - 8;

	if(index == selectedIndex) {
		drawSlectedText(0xA0, topPosition, textIndex, 0xF, 4);
	} else {
		drawText(0xA0, topPosition, textIndex, 4);
	}
}

void drawSystemMenuLayout(int selectedStringNumber) 
{

	drawAITDBox(160, 100, 320, 200);


	drawAITDBox(80, 55, 120, 70);

	//scaleDownImage(40,35,aux2);

	setClipSize(currentMenuLeft, currentMenuTop, currentMenuRight, currentMenuBottom);

	printString(0, 48, selectedStringNumber);
	printString(1, 45, selectedStringNumber);
	printString(2, 46, selectedStringNumber);
	printString(3, 41 + musicEnabled, selectedStringNumber);
	printString(4, 43/*+soundToggle*/, selectedStringNumber);
	printString(5, 49/*+detailToggle*/, selectedStringNumber);
	printString(6, 47, selectedStringNumber);

	menuWaitVSync();
}

void processSystemMenu(void) 
{
	int entry = -1;
	int exitMenu = 0;
	int currentSelectedEntry;

	freezeTime();
	//pauseShaking();

	if(lightVar1) 
	{
		//makeBlackPalette();
	}

	//clearScreenSystemMenu(unkScreenVar,aux2);

	currentSelectedEntry = 0;

	while(!exitMenu) 
	{
		readKeyboard();
		flushScreen();

		drawSystemMenuLayout(currentSelectedEntry);
		g_driver->CopyBlockPhys((unsigned char *)screen, 0, 0, 320, 200);
		g_driver->startFrame();

		if(lightVar1) 
		{
			make3dTatouUnk1(0x40, 0);
		}

		//  while(!exitMenu)
		{
			input3 = input2;
			button = input1;
			input4 = inputKey;

			if(!input5) 
			{
				if(input3 == 0x1C || button) 
				{ // enter
					switch(currentSelectedEntry) 
					{
					case 0: // exit menu
						exitMenu = 1;
						break;
					case 1: // save
						makeSave(45);
						break;
					case 2: // load
						if(restoreSave(46, 1)) 
						{
							mainVar1 = 2;
							unfreezeTime();
							//updateShaking();
							return;
						}
						break;

					}
				} 
				else 
				{
					if(input3 == 0x1B) 
					{
						exitMenu = 1;
					}
					if(input4 == 1) 
					{ // up
						currentSelectedEntry--;

						if(currentSelectedEntry < 0)
							currentSelectedEntry = 6;

						input5 = 1;
					}
					if(input4 == 2) 
					{ // bottom
						currentSelectedEntry++;

						if(currentSelectedEntry > 6)
							currentSelectedEntry = 0;

						input5 = 1;
					}
				}
			} 
			else 
			{
				if(!input3 && !input4) 
				{
					input5 = 0;
				}
			}
		}

		flipScreen();
	}
}

} // end of namespace Fitd
