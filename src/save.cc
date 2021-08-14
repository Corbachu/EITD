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

#define FORBIDDEN_SYMBOL_EXCEPTION_FILE
#define FORBIDDEN_SYMBOL_EXCEPTION_fopen
#define FORBIDDEN_SYMBOL_EXCEPTION_fclose
#define FORBIDDEN_SYMBOL_EXCEPTION_fread
#define FORBIDDEN_SYMBOL_EXCEPTION_fwrite
#define FORBIDDEN_SYMBOL_EXCEPTION_fseek
#define FORBIDDEN_SYMBOL_EXCEPTION_ftell

#include "fitd.h"
#include "common.h"

namespace Fitd {

struct saveEntry {
	void *ptr;
	uint32 size;
};

unsigned int currentSaveEntrySize;
saveEntry saveTable[40];

void *getSaveEntry(int index) {
	currentSaveEntrySize = saveTable[index].size;

	return(saveTable[index].ptr);
}

void loadRotateStruc(rotateStruct *pRotateStruct, FILE *fHandle) {
	ASSERT(sizeof(pRotateStruct->oldAngle) == 2);
	fread(&pRotateStruct->oldAngle, 2, 1, fHandle);

	ASSERT(sizeof(pRotateStruct->newAngle) == 2);
	fread(&pRotateStruct->newAngle, 2, 1, fHandle);

	ASSERT(sizeof(pRotateStruct->param) == 2);
	fread(&pRotateStruct->param, 2, 1, fHandle);

	ASSERT(sizeof(pRotateStruct->timeOfRotate) == 2);
	fread(&pRotateStruct->timeOfRotate, 2, 1, fHandle);
}

void saveRotateStruc(rotateStruct *pRotateStruct, FILE *fHandle) {
	ASSERT(sizeof(pRotateStruct->oldAngle) == 2);
	fwrite(&pRotateStruct->oldAngle, 2, 1, fHandle);

	ASSERT(sizeof(pRotateStruct->newAngle) == 2);
	fwrite(&pRotateStruct->newAngle, 2, 1, fHandle);

	ASSERT(sizeof(pRotateStruct->param) == 2);
	fwrite(&pRotateStruct->param, 2, 1, fHandle);

	ASSERT(sizeof(pRotateStruct->timeOfRotate) == 2);
	fwrite(&pRotateStruct->timeOfRotate, 2, 1, fHandle);
}

int loadSave(int saveNumber) {
	char buffer[256];
	FILE *fHandle;
	unsigned int var28;
	int var_14 = 0;
	char *ptr;
	int var_E;
	int var_16;
	unsigned int offsetToVars;
	unsigned short int tempVarSize;
	unsigned int offsetToActors;
	int i;
	int oldNumMaxObj;

	sprintf(buffer, "SAVE%d.ITD", saveNumber);

	fHandle = fopen(buffer, "rb");

	if(!fHandle) {
		return(0);
	}

	initEngine();
	initVars();

	fseek(fHandle, 8, SEEK_SET);

	fread(&var28, 4, 1, fHandle);

	var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | ((var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);

	fseek(fHandle, var28, SEEK_SET);

	ASSERT(sizeof(currentDisplayedRoom) == 2);
	fread(&currentDisplayedRoom, 2, 1, fHandle);

	ASSERT(sizeof(currentEtage) == 2);
	fread(&currentEtage, 2, 1, fHandle);

	ASSERT(sizeof(currentCamera) == 2);
	fread(&currentCamera, 2, 1, fHandle);

	ASSERT(sizeof(currentCameraTarget) == 2);
	fread(&currentCameraTarget, 2, 1, fHandle);

	ASSERT(sizeof(currentActorCameraTarget) == 2);
	fread(&currentActorCameraTarget, 2, 1, fHandle);

	ASSERT(sizeof(maxObjects) == 2);
	fread(&maxObjects, 2, 1, fHandle);

	if(g_fitd->getGameType() == GType_AITD1) {
		oldNumMaxObj = maxObjects;
		maxObjects = 300; // fix for save engine..
	}

	for(i = 0; i < maxObjects; i++) {
		ASSERT(sizeof(objectTable[i].ownerIdx) == 2);
		fread(&objectTable[i].ownerIdx, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].body) == 2);
		fread(&objectTable[i].body, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].flags) == 2);
		fread(&objectTable[i].flags, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].zvType) == 2);
		fread(&objectTable[i].zvType, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].foundBody) == 2);
		fread(&objectTable[i].foundBody, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].foundName) == 2);
		fread(&objectTable[i].foundName, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].flags2) == 2);
		fread(&objectTable[i].flags2, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].foundLife) == 2);
		fread(&objectTable[i].foundLife, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].x) == 2);
		fread(&objectTable[i].x, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].y) == 2);
		fread(&objectTable[i].y, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].z) == 2);
		fread(&objectTable[i].z, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].alpha) == 2);
		fread(&objectTable[i].alpha, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].beta) == 2);
		fread(&objectTable[i].beta, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].gamma) == 2);
		fread(&objectTable[i].gamma, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].stage) == 2);
		fread(&objectTable[i].stage, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].room) == 2);
		fread(&objectTable[i].room, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].lifeMode) == 2);
		fread(&objectTable[i].lifeMode, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].life) == 2);
		fread(&objectTable[i].life, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].field_24) == 2);
		fread(&objectTable[i].field_24, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].anim) == 2);
		fread(&objectTable[i].anim, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].frame) == 2);
		fread(&objectTable[i].frame, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].animType) == 2);
		fread(&objectTable[i].animType, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].animInfo) == 2);
		fread(&objectTable[i].animInfo, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].trackMode) == 2);
		fread(&objectTable[i].trackMode, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].trackNumber) == 2);
		fread(&objectTable[i].trackNumber, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].positionInTrack) == 2);
		fread(&objectTable[i].positionInTrack, 2, 1, fHandle);
	}

	if(g_fitd->getGameType() == GType_AITD1) {
		maxObjects = oldNumMaxObj;
	}

	if(g_fitd->getGameType() == GType_AITD1) {
		//ASSERT(numCVars == 45);
	}

	for(i = 0; i < g_fitd->getNumCVars(); i++) {
		ASSERT(sizeof(CVars[i]) == 2);
		fread(&CVars[i], 2, 1, fHandle);
	}

	ASSERT(sizeof(inHand) == 2);
	fread(&inHand, 2, 1, fHandle);

	ASSERT(sizeof(numObjInInventory) == 2);
	fread(&numObjInInventory, 2, 1, fHandle);

	if(g_fitd->getGameType() == GType_AITD1) {
		ASSERT(INVENTORY_SIZE == 30);
	}

	for(i = 0; i < INVENTORY_SIZE; i++) {
		ASSERT(sizeof(inventory[i]) == 2);
		fread(&inventory[i], 2, 1, fHandle);
	}

	ASSERT(sizeof(statusScreenAllowed) == 2);
	fread(&statusScreenAllowed, 2, 1, fHandle);

	ASSERT(sizeof(giveUp) == 2);
	fread(&giveUp, 2, 1, fHandle);

	ASSERT(sizeof(lightVar1) == 2);
	fread(&lightVar1, 2, 1, fHandle);

	ASSERT(sizeof(shakingAmplitude) == 2);
	fread(&shakingAmplitude, 2, 1, fHandle);

	ASSERT(sizeof(shakeVar1) == 2);
	fread(&shakeVar1, 2, 1, fHandle);

	uint32 tempTimer;
	ASSERT(sizeof(g_fitd->getTimer()) == 4);
	fread(&tempTimer, 4, 1, fHandle);
	g_fitd->setTimer(tempTimer);

	ASSERT(sizeof(timerFreeze1) == 4);
	fread(&timerFreeze1, 4, 1, fHandle);

	ASSERT(sizeof(currentMusic) == 2);
	fread(&currentMusic, 2, 1, fHandle);

	//timerFreeze = 1;

	var_E = currentCamera;

	loadFloor(currentEtage);
	currentCamera = -1;
	loadRoom(currentDisplayedRoom);
	var_16 = currentMusic;
	currentMusic = -1;
	playMusic(var_16);

	fseek(fHandle, 12, SEEK_SET);
	fread(&offsetToVars, 4, 1, fHandle);
	offsetToVars = ((offsetToVars & 0xFF) << 24) | ((offsetToVars & 0xFF00) << 8) | ((offsetToVars & 0xFF0000) >> 8) | ((offsetToVars & 0xFF000000) >> 24);
	fseek(fHandle, offsetToVars, SEEK_SET);

	fread(&tempVarSize, 2, 1, fHandle);
	varSize = tempVarSize;

	fread(vars, varSize, 1, fHandle);

	if(g_fitd->getGameType() == GType_AITD1) {
		configureHqrHero(listBody, listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
		configureHqrHero(listAnim, listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
	} else {
		configureHqrHero(listBody, 0);
		configureHqrHero(listAnim, 0);
	}

	fseek(fHandle, 16, SEEK_SET);
	fread(&offsetToActors, 4, 1, fHandle);
	offsetToVars = ((offsetToActors & 0xFF) << 24) | ((offsetToActors & 0xFF00) << 8) | ((offsetToActors & 0xFF0000) >> 8) | ((offsetToActors & 0xFF000000) >> 24);
	fseek(fHandle, offsetToVars, SEEK_SET);

	for(i = 0; i < NUM_MAX_ACTOR; i++) {
		ASSERT(sizeof(actorTable[i].ID) == 2);
		fread(&actorTable[i].ID, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].bodyNum) == 2);
		fread(&actorTable[i].bodyNum, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].flags) == 2);
		fread(&actorTable[i].flags, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].dynFlags) == 2);
		fread(&actorTable[i].dynFlags, 2, 1, fHandle);

		//    ASSERT(sizeof(actorTable[i].zv.ZVX1) == 2);
		fread(&actorTable[i].zv.ZVX1, 2, 1, fHandle);
		actorTable[i].zv.ZVX1 = (short int)actorTable[i].zv.ZVX1;

		//    ASSERT(sizeof(actorTable[i].zv.ZVX2) == 2);
		fread(&actorTable[i].zv.ZVX2, 2, 1, fHandle);
		actorTable[i].zv.ZVX2 = (short int)actorTable[i].zv.ZVX2;

		//    ASSERT(sizeof(actorTable[i].zv.ZVY1) == 2);
		fread(&actorTable[i].zv.ZVY1, 2, 1, fHandle);
		actorTable[i].zv.ZVY1 = (short int)actorTable[i].zv.ZVY1;

		//    ASSERT(sizeof(actorTable[i].zv.ZVY2) == 2);
		fread(&actorTable[i].zv.ZVY2, 2, 1, fHandle);
		actorTable[i].zv.ZVY2 = (short int)actorTable[i].zv.ZVY2;

		//    ASSERT(sizeof(actorTable[i].zv.ZVZ1) == 2);
		fread(&actorTable[i].zv.ZVZ1, 2, 1, fHandle);
		actorTable[i].zv.ZVZ1 = (short int)actorTable[i].zv.ZVZ1;

		//    ASSERT(sizeof(actorTable[i].zv.ZVZ2) == 2);
		fread(&actorTable[i].zv.ZVZ2, 2, 1, fHandle);
		actorTable[i].zv.ZVZ2 = (short int)actorTable[i].zv.ZVZ2;

		ASSERT(sizeof(actorTable[i].BBox3D1) == 2);
		fread(&actorTable[i].BBox3D1, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D2) == 2);
		fread(&actorTable[i].BBox3D2, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D3) == 2);
		fread(&actorTable[i].BBox3D3, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D4) == 2);
		fread(&actorTable[i].BBox3D4, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].roomX) == 2);
		fread(&actorTable[i].roomX, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].roomY) == 2);
		fread(&actorTable[i].roomY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].roomZ) == 2);
		fread(&actorTable[i].roomZ, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].worldX) == 2);
		fread(&actorTable[i].worldX, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].worldY) == 2);
		fread(&actorTable[i].worldY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].worldZ) == 2);
		fread(&actorTable[i].worldZ, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].alpha) == 2);
		fread(&actorTable[i].alpha, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].beta) == 2);
		fread(&actorTable[i].beta, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].gamma) == 2);
		fread(&actorTable[i].gamma, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].stage) == 2);
		fread(&actorTable[i].stage, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].room) == 2);
		fread(&actorTable[i].room, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].lifeMode) == 2);
		fread(&actorTable[i].lifeMode, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].life) == 2);
		fread(&actorTable[i].life, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].CHRONO) == 4);
		fread(&actorTable[i].CHRONO, 4, 1, fHandle);

		ASSERT(sizeof(actorTable[i].ROOM_CHRONO) == 4);
		fread(&actorTable[i].ROOM_CHRONO, 4, 1, fHandle);

		ASSERT(sizeof(actorTable[i].ANIM) == 2);
		fread(&actorTable[i].ANIM, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animType) == 2);
		fread(&actorTable[i].animType, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animInfo) == 2);
		fread(&actorTable[i].animInfo, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].nextAnim) == 2);
		fread(&actorTable[i].nextAnim, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].nextAnimType) == 2);
		fread(&actorTable[i].nextAnimType, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].nextAnimInfo) == 2);
		fread(&actorTable[i].nextAnimInfo, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].FRAME) == 2);
		fread(&actorTable[i].FRAME, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].FRAME_COUNT) == 2);
		fread(&actorTable[i].FRAME_COUNT, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].END_FRAME) == 2);
		fread(&actorTable[i].END_FRAME, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].END_ANIM) == 2);
		fread(&actorTable[i].END_ANIM, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].trackMode) == 2);
		fread(&actorTable[i].trackMode, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].trackNumber) == 2);
		fread(&actorTable[i].trackNumber, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].MARK) == 2);
		fread(&actorTable[i].MARK, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].positionInTrack) == 2);
		fread(&actorTable[i].positionInTrack, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].modX) == 2);
		fread(&actorTable[i].modX, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].modY) == 2);
		fread(&actorTable[i].modY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].modZ) == 2); // 45
		fread(&actorTable[i].modZ, 2, 1, fHandle);

		loadRotateStruc(&actorTable[i].fall, fHandle);

		ASSERT(sizeof(actorTable[i].falling) == 2);
		fread(&actorTable[i].falling, 2, 1, fHandle);

		loadRotateStruc(&actorTable[i].rotate, fHandle);

		ASSERT(sizeof(actorTable[i].rotateTarget) == 2);
		fread(&actorTable[i].rotateTarget, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].speed) == 2);
		fread(&actorTable[i].speed, 2, 1, fHandle);

		loadRotateStruc(&actorTable[i].speedChange, fHandle);

		ASSERT(sizeof(actorTable[i].COL[0]) == 2);
		fread(&actorTable[i].COL[0], 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].COL[1]) == 2);
		fread(&actorTable[i].COL[1], 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].COL[2]) == 2);
		fread(&actorTable[i].COL[2], 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].COL_BY) == 2);
		fread(&actorTable[i].COL_BY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HARD_DEC) == 2);
		fread(&actorTable[i].HARD_DEC, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HARD_COL) == 2);
		fread(&actorTable[i].HARD_COL, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HIT) == 2);
		fread(&actorTable[i].HIT, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HIT_BY) == 2);
		fread(&actorTable[i].HIT_BY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionType) == 2);
		fread(&actorTable[i].animActionType, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionANIM) == 2);
		fread(&actorTable[i].animActionANIM, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionFRAME) == 2);
		fread(&actorTable[i].animActionFRAME, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionParam) == 2);
		fread(&actorTable[i].animActionParam, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hitForce) == 2);
		fread(&actorTable[i].hitForce, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionHotpoint) == 2);
		fread(&actorTable[i].animActionHotpoint, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hotPoint.x) == 2);
		fread(&actorTable[i].hotPoint.x, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hotPoint.y) == 2);
		fread(&actorTable[i].hotPoint.x, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hotPoint.z) == 2);
		fread(&actorTable[i].hotPoint.x, 2, 1, fHandle);
	}
	fclose(fHandle);

	for(i = 0; i < NUM_MAX_ACTOR; i++) {
		if(actorTable[i].ID != -1 && actorTable[i].bodyNum != -1) {
			char *bodyPtr = listBody->get(actorTable[i].bodyNum);

			if(actorTable[i].ANIM != -1) {
				char *animPtr = listAnim->get(actorTable[i].ANIM);
				initAnimInBody(actorTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	startGameVar1 = var_E;

	return(1);
}

int restoreSave(int arg0, int arg1) {
	int selectedSave;
	//  restoreSaveVar1 = arg0;

	if(arg1 == 0) {
		flushScreen();
		flipScreen();
		make3dTatouUnk1(0x40, 0);
	}

	selectedSave = parseAllSaves(0);

	if(arg1 == 0) {
		//  fadeOut(8,0);
	}

	if(selectedSave == -1) {
		return(0);
	}

	if(arg1 == 0) {
		//freeScene();
	}

	return(loadSave(selectedSave));

}

int makeSaveFile(int entry) {
	FILE *fHandle;
	char buffer[100];
	int i;
	unsigned long int var28 = 0;
	char *ptr;
	int var_14 = 0;
	int temp;
	int oldNumMaxObj;

	for(i = 0; i < NUM_MAX_ACTOR; i++) {
		if(actorTable[i].ID == -1);

		if(actorTable[i].ANIM == 4) {
			CVars[getCVarsIdx(FOG_FLAG)] = 0;
			printTextSub6(hqrUnk, actorTable[i].FRAME);
		}
	}

	sprintf(buffer, "SAVE%d.ITD", entry);

	fHandle = fopen(buffer, "wb+");

	if(!fHandle)
		return 0;

	fwrite(&var28, 4, 1, fHandle);
	fwrite(&var28, 4, 1, fHandle);

	var28 = ftell(fHandle) + 12;
	var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | ((var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);

	fwrite(&var28, 4, 1, fHandle);

	fwrite(&var28, 4, 1, fHandle);
	fwrite(&var28, 4, 1, fHandle);

	ASSERT(sizeof(currentDisplayedRoom) == 2);
	fwrite(&currentDisplayedRoom, 2, 1, fHandle);

	ASSERT(sizeof(currentEtage) == 2);
	fwrite(&currentEtage, 2, 1, fHandle);

	ASSERT(sizeof(currentCamera) == 2);
	fwrite(&currentCamera, 2, 1, fHandle);

	ASSERT(sizeof(currentCameraTarget) == 2);
	fwrite(&currentCameraTarget, 2, 1, fHandle);

	ASSERT(sizeof(currentActorCameraTarget) == 2);
	fwrite(&currentActorCameraTarget, 2, 1, fHandle);

	ASSERT(sizeof(maxObjects) == 2);
	fwrite(&maxObjects, 2, 1, fHandle);

	if(g_fitd->getGameType() == GType_AITD1) {
		oldNumMaxObj = maxObjects;
		maxObjects = 300; // fix for save engine..
	}

	for(i = 0; i < maxObjects; i++) {
		ASSERT(sizeof(objectTable[i].ownerIdx) == 2);
		fwrite(&objectTable[i].ownerIdx, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].body) == 2);
		fwrite(&objectTable[i].body, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].flags) == 2);
		fwrite(&objectTable[i].flags, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].zvType) == 2);
		fwrite(&objectTable[i].zvType, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].foundBody) == 2);
		fwrite(&objectTable[i].foundBody, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].foundName) == 2);
		fwrite(&objectTable[i].foundName, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].flags2) == 2);
		fwrite(&objectTable[i].flags2, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].foundLife) == 2);
		fwrite(&objectTable[i].foundLife, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].x) == 2);
		fwrite(&objectTable[i].x, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].y) == 2);
		fwrite(&objectTable[i].y, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].z) == 2);
		fwrite(&objectTable[i].z, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].alpha) == 2);
		fwrite(&objectTable[i].alpha, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].beta) == 2);
		fwrite(&objectTable[i].beta, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].gamma) == 2);
		fwrite(&objectTable[i].gamma, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].stage) == 2);
		fwrite(&objectTable[i].stage, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].room) == 2);
		fwrite(&objectTable[i].room, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].lifeMode) == 2);
		fwrite(&objectTable[i].lifeMode, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].life) == 2);
		fwrite(&objectTable[i].life, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].field_24) == 2);
		fwrite(&objectTable[i].field_24, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].anim) == 2);
		fwrite(&objectTable[i].anim, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].frame) == 2);
		fwrite(&objectTable[i].frame, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].animType) == 2);
		fwrite(&objectTable[i].animType, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].animInfo) == 2);
		fwrite(&objectTable[i].animInfo, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].trackMode) == 2);
		fwrite(&objectTable[i].trackMode, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].trackNumber) == 2);
		fwrite(&objectTable[i].trackNumber, 2, 1, fHandle);

		ASSERT(sizeof(objectTable[i].positionInTrack) == 2);
		fwrite(&objectTable[i].positionInTrack, 2, 1, fHandle);
	}

	if(g_fitd->getGameType() == GType_AITD1) {
		maxObjects = oldNumMaxObj;
	}

	if(g_fitd->getGameType() == GType_AITD1) {
		//ASSERT(numCVars == 45);
	}

	for(i = 0; i < g_fitd->getNumCVars(); i++) {
		ASSERT(sizeof(CVars[i]) == 2);
		fwrite(&CVars[i], 2, 1, fHandle);
	}

	ASSERT(sizeof(inHand) == 2);
	fwrite(&inHand, 2, 1, fHandle);

	ASSERT(sizeof(numObjInInventory) == 2);
	fwrite(&numObjInInventory, 2, 1, fHandle);

	if(g_fitd->getGameType() == GType_AITD1) {
		ASSERT(INVENTORY_SIZE == 30);
	}

	for(i = 0; i < INVENTORY_SIZE; i++) {
		ASSERT(sizeof(inventory[i]) == 2);
		fwrite(&inventory[i], 2, 1, fHandle);
	}

	ASSERT(sizeof(statusScreenAllowed) == 2);
	fwrite(&statusScreenAllowed, 2, 1, fHandle);

	ASSERT(sizeof(giveUp) == 2);
	fwrite(&giveUp, 2, 1, fHandle);

	ASSERT(sizeof(lightVar1) == 2);
	fwrite(&lightVar1, 2, 1, fHandle);

	ASSERT(sizeof(shakingAmplitude) == 2);
	fwrite(&shakingAmplitude, 2, 1, fHandle);

	ASSERT(sizeof(shakeVar1) == 2);
	fwrite(&shakeVar1, 2, 1, fHandle);

	uint32 tempTimer = g_fitd->getTimer();
	ASSERT(sizeof(g_fitd->getTimer()) == 4);
	fwrite(&tempTimer, 4, 1, fHandle);

	ASSERT(sizeof(timerFreeze1) == 4);
	fwrite(&timerFreeze1, 4, 1, fHandle);

	ASSERT(sizeof(currentMusic) == 2);
	fwrite(&currentMusic, 2, 1, fHandle);

	//timerFreeze = 1;

	var28 = ftell(fHandle);
	temp = var28;
	fseek(fHandle, 12, SEEK_SET);
	var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | ((var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);
	fwrite(&var28, 4, 1, fHandle);
	fseek(fHandle, temp, SEEK_SET);

	fwrite(&varSize, 2, 1, fHandle);
	fwrite(vars, varSize, 1, fHandle);

	var28 = ftell(fHandle);
	temp = var28;
	fseek(fHandle, 16, SEEK_SET);
	var28 = ((var28 & 0xFF) << 24) | ((var28 & 0xFF00) << 8) | ((var28 & 0xFF0000) >> 8) | ((var28 & 0xFF000000) >> 24);
	fwrite(&var28, 4, 1, fHandle);
	fseek(fHandle, temp, SEEK_SET);

	for(i = 0; i < NUM_MAX_ACTOR; i++) {

		ASSERT(sizeof(actorTable[i].ID) == 2);
		fwrite(&actorTable[i].ID, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].bodyNum) == 2);
		fwrite(&actorTable[i].bodyNum, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].flags) == 2);
		fwrite(&actorTable[i].flags, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].dynFlags) == 2);
		fwrite(&actorTable[i].dynFlags, 2, 1, fHandle);

		//    ASSERT(sizeof(actorTable[i].zv.ZVX1) == 2);
		fwrite(&actorTable[i].zv.ZVX1, 2, 1, fHandle);
		actorTable[i].zv.ZVX1 = (short int)actorTable[i].zv.ZVX1;

		//    ASSERT(sizeof(actorTable[i].zv.ZVX2) == 2);
		fwrite(&actorTable[i].zv.ZVX2, 2, 1, fHandle);
		actorTable[i].zv.ZVX2 = (short int)actorTable[i].zv.ZVX2;

		//    ASSERT(sizeof(actorTable[i].zv.ZVY1) == 2);
		fwrite(&actorTable[i].zv.ZVY1, 2, 1, fHandle);

		//    ASSERT(sizeof(actorTable[i].zv.ZVY2) == 2);
		fwrite(&actorTable[i].zv.ZVY2, 2, 1, fHandle);

		//    ASSERT(sizeof(actorTable[i].zv.ZVZ1) == 2);
		fwrite(&actorTable[i].zv.ZVZ1, 2, 1, fHandle);

		//    ASSERT(sizeof(actorTable[i].zv.ZVZ2) == 2);
		fwrite(&actorTable[i].zv.ZVZ2, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D1) == 2);
		fwrite(&actorTable[i].BBox3D1, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D2) == 2);
		fwrite(&actorTable[i].BBox3D2, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D3) == 2);
		fwrite(&actorTable[i].BBox3D3, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].BBox3D4) == 2);
		fwrite(&actorTable[i].BBox3D4, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].roomX) == 2);
		fwrite(&actorTable[i].roomX, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].roomY) == 2);
		fwrite(&actorTable[i].roomY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].roomZ) == 2);
		fwrite(&actorTable[i].roomZ, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].worldX) == 2);
		fwrite(&actorTable[i].worldX, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].worldY) == 2);
		fwrite(&actorTable[i].worldY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].worldZ) == 2);
		fwrite(&actorTable[i].worldZ, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].alpha) == 2);
		fwrite(&actorTable[i].alpha, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].beta) == 2);
		fwrite(&actorTable[i].beta, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].gamma) == 2);
		fwrite(&actorTable[i].gamma, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].stage) == 2);
		fwrite(&actorTable[i].stage, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].room) == 2);
		fwrite(&actorTable[i].room, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].lifeMode) == 2);
		fwrite(&actorTable[i].lifeMode, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].life) == 2);
		fwrite(&actorTable[i].life, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].CHRONO) == 4);
		fwrite(&actorTable[i].CHRONO, 4, 1, fHandle);

		ASSERT(sizeof(actorTable[i].ROOM_CHRONO) == 4);
		fwrite(&actorTable[i].ROOM_CHRONO, 4, 1, fHandle);

		ASSERT(sizeof(actorTable[i].ANIM) == 2);
		fwrite(&actorTable[i].ANIM, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animType) == 2);
		fwrite(&actorTable[i].animType, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animInfo) == 2);
		fwrite(&actorTable[i].animInfo, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].nextAnim) == 2);
		fwrite(&actorTable[i].nextAnim, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].nextAnimType) == 2);
		fwrite(&actorTable[i].nextAnimType, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].nextAnimInfo) == 2);
		fwrite(&actorTable[i].nextAnimInfo, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].FRAME) == 2);
		fwrite(&actorTable[i].FRAME, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].FRAME_COUNT) == 2);
		fwrite(&actorTable[i].FRAME_COUNT, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].END_FRAME) == 2);
		fwrite(&actorTable[i].END_FRAME, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].END_ANIM) == 2);
		fwrite(&actorTable[i].END_ANIM, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].trackMode) == 2);
		fwrite(&actorTable[i].trackMode, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].trackNumber) == 2);
		fwrite(&actorTable[i].trackNumber, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].MARK) == 2);
		fwrite(&actorTable[i].MARK, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].positionInTrack) == 2);
		fwrite(&actorTable[i].positionInTrack, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].modX) == 2);
		fwrite(&actorTable[i].modX, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].modY) == 2);
		fwrite(&actorTable[i].modY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].modZ) == 2); // 45
		fwrite(&actorTable[i].modZ, 2, 1, fHandle);

		saveRotateStruc(&actorTable[i].fall, fHandle);

		ASSERT(sizeof(actorTable[i].falling) == 2);
		fwrite(&actorTable[i].falling, 2, 1, fHandle);

		saveRotateStruc(&actorTable[i].rotate, fHandle);

		ASSERT(sizeof(actorTable[i].rotateTarget) == 2);
		fwrite(&actorTable[i].rotateTarget, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].speed) == 2);
		fwrite(&actorTable[i].speed, 2, 1, fHandle);

		saveRotateStruc(&actorTable[i].speedChange, fHandle);

		ASSERT(sizeof(actorTable[i].COL[0]) == 2);
		fwrite(&actorTable[i].COL[0], 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].COL[1]) == 2);
		fwrite(&actorTable[i].COL[1], 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].COL[2]) == 2);
		fwrite(&actorTable[i].COL[2], 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].COL_BY) == 2);
		fwrite(&actorTable[i].COL_BY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HARD_DEC) == 2);
		fwrite(&actorTable[i].HARD_DEC, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HARD_COL) == 2);
		fwrite(&actorTable[i].HARD_COL, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HIT) == 2);
		fwrite(&actorTable[i].HIT, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].HIT_BY) == 2);
		fwrite(&actorTable[i].HIT_BY, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionType) == 2);
		fwrite(&actorTable[i].animActionType, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionANIM) == 2);
		fwrite(&actorTable[i].animActionANIM, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionFRAME) == 2);
		fwrite(&actorTable[i].animActionFRAME, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionParam) == 2);
		fwrite(&actorTable[i].animActionParam, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hitForce) == 2);
		fwrite(&actorTable[i].hitForce, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].animActionHotpoint) == 2);
		fwrite(&actorTable[i].animActionHotpoint, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hotPoint.x) == 2);
		fwrite(&actorTable[i].hotPoint.x, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hotPoint.y) == 2);
		fwrite(&actorTable[i].hotPoint.x, 2, 1, fHandle);

		ASSERT(sizeof(actorTable[i].hotPoint.z) == 2);
		fwrite(&actorTable[i].hotPoint.x, 2, 1, fHandle);
	}

	fclose(fHandle);

	return 1;
}

int makeSave(int arg0) {
	return(makeSaveFile(0));
}

} // end of namespace Fitd
