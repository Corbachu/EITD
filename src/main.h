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

#ifndef _MAIN_
#define _MAIN_
namespace Fitd {

struct hqrEntryStruct;

extern int input5;

void menuWaitVSync();
void printTextSub5(int x, int y, int param, char *gfx);
void flipScreen();
void loadFloor(int floorNumber);
void loadRoom(int roomNumber);
void updateInHand(int objIdx);
void checkIfCameraChangeIsRequired(void);
void updateAllActorAndObjects();
void createActorList();
void getZvNormal(char *bodyPtr, ZVStruct *zvPtr);
int checkForHardCol(ZVStruct *zvPtr, roomDataStruct *pRoomData);
void walkStep(int angle1, int angle2, int angle3);
void throwStoppedAt(int x, int z);
void initEngine(void);
void initVars();
void configureHqrHero(hqrEntryStruct *hqrPtr, const char *name);
void flushScreen(void);
int parseAllSaves(int arg);

void cleanupAndExit(void);
void drawProjectedBox(int x1, int x2, int y1, int y2, int z1, int z2, int color, int transparency);
void printTextSub6(hqrEntryStruct *hqrPtr, int index);

extern char scaledScreen[640*400];

void sysInit(void);
void freeAll(void);
void startGame(int startupEtage, int startupRoom, int allowSystemMenu);
void fillBox(int x1, int y1, int x2, int y2, char color); // fast recode. No RE
void copyZv(ZVStruct *source, ZVStruct *dest);
void getZvRelativePosition(ZVStruct *zvPtr, int startRoom, int destRoom);
void take(int objIdx);
void foundObject(int objIdx, int param);
void hit(int animNumber, int arg_2, int arg_4, int arg_6, int hitForce, int arg_A);
int checkZvCollision(ZVStruct *zvPtr1, ZVStruct *zvPtr2);
void getZvCube(char *bodyPtr, ZVStruct *zvPtr);
void putAt(int objIdx, int objIdxToPutAt);
void setClipSize(int left, int top, int right, int bottom);

///
int printText(int index, int left, int top, int right, int bottom, int mode, int color);

void setupSMCode(int centerX, int centerY, int x, int y, int z);
void setupPointTransformSM(int x, int y, int z);
void setupSelfModifyingCode(int x, int y, int z);
short int computeDistanceToPoint(int x1, int z1, int x2, int z2);
void setMoveMode(int trackMode, int trackNumber);
void startActorRotation(short int beta, short int newBeta, short int param, rotateStruct *rotatePtr);
short int updateActorRotation(rotateStruct *rotatePtr);
void deleteObject(int objIdx);
void deleteSub(int actorIdx);
void stopAnim(int actorIdx);
void makeMessage(int messageIdx);
int drawTextOverlay(void);

void getZvRot(char *bodyPtr, ZVStruct *zvPtr, int alpha, int beta, int gamma);

} // end of namespace Fitd


#endif

