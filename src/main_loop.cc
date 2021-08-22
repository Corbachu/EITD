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

#include "fitd.h"
#include "actor.h"
#include "common.h"
#include "gfx.h"

namespace Fitd 
{

int mainLoopSwitch = 0;

void updatePendingEvents(void) 
{
	// TODO: miss pending events here

	if(currentMusic != -1) 
	{
		if(currentMusic == -2) 
		{
			if(evalChrono(&musicChrono) > 180) 
			{
				playMusic(nextMusic);
			}
		} else {
			if(fadeMusic(0, 0, 0x10) == -1) 
			{
				currentMusic = -1;

				if(nextMusic != -1) 
				{
					playMusic(nextMusic);
					nextMusic = -1;
				}
			}
		}
	}
}

void mainLoop(int allowSystemMenu) 
{
#define SPEED 15              /* Ticks per Frame */
#define SLEEP_MIN 20          /* Minimum time a sleep takes, usually 2*GRAN */
#define SLEEP_GRAN 1         /* Granularity of sleep */

	int frames = 0;                 /* Number of frames displayed */
	int32 t_start, t_left;
	uint32 t_end;
	int32 q = 0;                   /* Dummy */

	while(input2 == 0x1B || input2 == 0x1C) 
	{
		readKeyboard();
	}

	//atexit(cleanupAndExit);

	while(1) 
	{
		frames++;
		t_start = g_fitd->getTicks();

		timeGlobal++;

		g_fitd->setTimer(timeGlobal);

		process_events();
		readKeyboard();
		input3 = input2;
		input4 = inputKey;
		button = input1;

		if(input3) 
		{
			if(input3 == 0x1B) 
			{
				while(input2 == 0x1B) 
				{
					readKeyboard();
				}
				processSystemMenu();
				while(input2 == 0x1B || input2 == 0x1C) 
				{
					readKeyboard();
					input3 = input2;
				}
			}

			if(input3 == 0x1C || input3 == 0x17) 
			{
				if(allowSystemMenu == 0) 
				{
					break;
				}

				if(statusScreenAllowed) 
				{
					makeStatusScreen();
				}
			}
		} else {
			//      input5 = 0;
		}

		if(button) 
		{
			if(!allowSystemMenu) 
			{
				break;
			}

			action = 0x2000;
		} else 
		{
			action = 0;
		}

		if(g_fitd->getGameType() == GType_AITD1) 
		{
			updateInHand(inHand);
		} else 
		{
			if(g_fitd->getGameType() == GType_AITD2) // seems to crash in Jack for now
				updateInHand(inHandTable[currentInHand]);
		}

		if(changeFloor == 0) 
		{
			if(g_fitd->getGameType() == GType_AITD1) 
			{
				if(CVars[getCVarsIdx(LIGHT_OBJECT)] == -1) 
				{
					//        lightPosY = 2000;
					//        lightPosX = 2000;
				}
			}

			currentProcessedActorPtr = actorTable;

			for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_ACTOR; currentProcessedActorIdx++) 
			{
				if(currentProcessedActorPtr->ID >= 0) 
				{
					currentProcessedActorPtr->COL_BY = -1;
					currentProcessedActorPtr->HIT_BY = -1;
					currentProcessedActorPtr->HIT = -1;
					currentProcessedActorPtr->HARD_DEC = -1;
					currentProcessedActorPtr->HARD_COL = -1;
				}

				currentProcessedActorPtr++;
			}

			currentProcessedActorPtr = actorTable;
			for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_ACTOR; currentProcessedActorIdx++) 
			{
				if(currentProcessedActorPtr->ID >= 0) 
				{
					int flag = currentProcessedActorPtr->flags;

					if(flag & 1 || (g_fitd->getGameType() >= GType_AITD2 && flag & 0x200)) 
					{
						processActor1();
					}

					if(flag & 0x40) 
					{
						processActor2();
					}

					if(currentProcessedActorPtr->animActionType) 
					{
						processAnimAction();
					}
				}

				currentProcessedActorPtr++;
			}

			currentProcessedActorPtr = actorTable;
			for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_ACTOR; currentProcessedActorIdx++) 
			{
				if(currentProcessedActorPtr->ID >= 0) 
				{
					if(currentProcessedActorPtr->life != -1) 
					{
						switch(g_fitd->getGameType()) 
						{
						case GType_AITD2:
						case GType_AITD3: 
						{
							if(currentProcessedActorPtr->lifeMode & 3)
								if(!(currentProcessedActorPtr->lifeMode & 4))
									processLife(currentProcessedActorPtr->life);
							break;
						}
						case GType_JACK: 
						{
							if(currentProcessedActorPtr->life != -1)
								if(currentProcessedActorPtr->lifeMode != -1)
									processLife(currentProcessedActorPtr->life);
							break;
						}
						case GType_AITD1: 
						{
							if(currentProcessedActorPtr->life != -1)
								if(currentProcessedActorPtr->lifeMode != -1)
									processLife(currentProcessedActorPtr->life);
							break;
						}
						}
					}
				}

				if(changeFloor)
					break;

				currentProcessedActorPtr++;
			}

			if(giveUp)
				break;
		}

		if(changeFloor) 
		{
			loadFloor(newFloor);
		}

#if INTERNAL_DEBUGGER
		if(debuggerVar_topCamera)
			setupCamera();
#endif

		if(needChangeRoom) 
		{
			loadRoom(newRoom);
			setupCamera();
		} else {
			checkIfCameraChangeIsRequired();
			if(g_fitd->getGameType() >= GType_AITD2) 
			{
				int tempCurrentCamera;

				tempCurrentCamera = currentCamera;

				currentCamera = startGameVar1;

				for(currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_ACTOR; currentProcessedActorIdx++) 
				{
					if(currentProcessedActorPtr->ID >= 0) 
					{
						if(currentProcessedActorPtr->life != -1) 
						{
							if(currentProcessedActorPtr->flags & 0x200) 
							{
								if(currentProcessedActorPtr->lifeMode & 3)
									if(!(currentProcessedActorPtr->lifeMode & 4)) 
									{
										processLife(currentProcessedActorPtr->life);
										actorTurnedToObj = 1;
									}
							}
						}
					}

					if(changeFloor)
						break;
				}

				if(giveUp)
					break;

				currentCamera = tempCurrentCamera;
			}
			if(mainVar1) 
			{
				setupCamera();
			}
		}

		//    if(objModifFlag1)
		{
			updateAllActorAndObjects();
		}

		//    if(actorTurnedToObj)
		{
			createActorList();
		}

		sortActorList();

		//    if(objModifFlag2)
		{
			//      setupCameraSub4();
		}

		//    mainLoopSub1();

		mainDraw(setupCameraVar1);

		//osystem_delay(100);

		updatePendingEvents();

		t_end = t_start + SPEED;
		t_left = t_start - g_fitd->getTicks() + SPEED;

		if(t_left > 0) 
		{
			if(t_left > SLEEP_MIN)
				g_fitd->delay(t_left - SLEEP_GRAN);
			while(g_fitd->getTicks() < t_end) 
			{
				q++;
			};
		} else 
		{
			// printf("CPU to slow by %d ticks/round\n",-t_left);
		};
	}

	//  mainLoopVar1 = 0;
	//  shakingState = 0;

	//  stopShaking();
	//  stopSounds();
}

} // end of namespace Fitd
