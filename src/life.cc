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

//#include "common/textconsole.h" w32_system
#include "epi/asserts.h"
#include "fitd.h"
#include "osystem.h"
#include "common.h"

namespace Fitd {

int groundLevel;
int16 specialTable[4] = {144, 192, 48, 112};

int16 numObjInInventoryTable[2];
int16 inHandTable[2];
int16 currentInHand = 0;

int numSequenceParam = 0;

//QUICK-fix, prototype this:
void playSequence(int sequenceIdx, int fadeStart, int fadeOutVar);

sequenceParamStruct sequenceParams[NUM_MAX_SEQUENCE_PARAM];

void resetRotateParam(void) {
	currentProcessedActorPtr->rotate.param = 0;
}

void throwObj(int animThrow, int frameThrow, int arg_4, int objToThrowIdx, int throwRotated, int throwForce, int animNext) {
	objectStruct *objPtr = &objectTable[objToThrowIdx];

	if(anim(animThrow, 2, animNext)) {
		currentProcessedActorPtr->animActionANIM = animThrow;
		currentProcessedActorPtr->animActionFRAME = frameThrow;
		currentProcessedActorPtr->animActionType = 6;
		currentProcessedActorPtr->animActionHotpoint = arg_4;
		currentProcessedActorPtr->animActionParam = objToThrowIdx;
		currentProcessedActorPtr->hitForce = throwForce;

		if(!throwRotated) {
			objectTable[objToThrowIdx].gamma -= 0x100;
		}

		objectTable[objToThrowIdx].flags2 |= 0x1000;
	}
}

void put(int x, int y, int z, int room, int stage, int alpha, int beta, int gamma, int idx) {
	objectStruct *objPtr = &objectTable[idx];

	objPtr->x = x;
	objPtr->y = y;
	objPtr->z = z;

	objPtr->room = room;
	objPtr->stage = stage;

	objPtr->alpha = alpha;
	objPtr->beta = beta;
	objPtr->gamma = gamma;

	removeObjFromInventory(idx);

	objPtr->flags2 |= 0x4000;

	/*  objModifFlag1 = 1;
	 objModifFlag2 = 1; */
}

void fire(int fireAnim, int X, int Y, int Z, int hitForce, int nextAnim) {
	if(anim(fireAnim, 2, nextAnim)) {
		currentProcessedActorPtr->animActionANIM = fireAnim;
		currentProcessedActorPtr->animActionFRAME = X;
		currentProcessedActorPtr->animActionType = 4;
		currentProcessedActorPtr->animActionParam = Z;
		currentProcessedActorPtr->animActionHotpoint = Y;
		currentProcessedActorPtr->hitForce = hitForce;

	}
}

int createFlow(int mode, int X, int Y, int Z, int stage, int room, int alpha, int beta, int gamma, ZVStruct *zvPtr) {
	int16 localSpecialTable[4];
	actorStruct *currentActorPtr;
	int i;
	ZVStruct *actorZvPtr;

	memcpy(localSpecialTable, specialTable, 8);

	currentActorPtr = actorTable;

	for(i = 0; i < NUM_MAX_ACTOR; i++) { // count the number of active actors
		if(currentActorPtr->ID == -1)
			break;
		currentActorPtr++;
	}

	if(i == NUM_MAX_ACTOR) { // no free actor entry, abort
		return(-1);
	}

	currentActorPtr->flags = 0x20;
	currentActorPtr->ID = -2;
	currentActorPtr->life = -1;
	currentActorPtr->lifeMode = 2;
	currentActorPtr->bodyNum = 0;
	currentActorPtr->worldX = currentActorPtr->roomX = X;
	currentActorPtr->worldY = currentActorPtr->roomY = Y;
	currentActorPtr->worldZ = currentActorPtr->roomZ = Z;
	currentActorPtr->stage = stage;
	currentActorPtr->room = room;

	if(currentDisplayedRoom != room) {
		currentActorPtr->worldX -= (int16)((roomDataTable[currentDisplayedRoom].worldX - roomDataTable[room].worldX) * 10);
		currentActorPtr->worldY += (int16)((roomDataTable[currentDisplayedRoom].worldY - roomDataTable[room].worldY) * 10);
		currentActorPtr->worldZ += (int16)((roomDataTable[currentDisplayedRoom].worldZ - roomDataTable[room].worldZ) * 10);
	}

	currentActorPtr->alpha = alpha;
	currentActorPtr->beta = beta;
	currentActorPtr->gamma = gamma;
	currentActorPtr->modX = 0;
	currentActorPtr->modY = 0;
	currentActorPtr->modZ = 0;

	actorZvPtr = &currentActorPtr->zv;

	copyZv(zvPtr, actorZvPtr);

	switch(mode) {
	case 0: {
		char *flowPtr;
		int j;

		actorZvPtr->ZVX1 -= X;
		actorZvPtr->ZVX2 -= X;
		actorZvPtr->ZVY1 -= Y;
		actorZvPtr->ZVY2 -= Y;
		actorZvPtr->ZVZ1 -= Z;
		actorZvPtr->ZVZ2 -= Z;

		currentActorPtr->FRAME = hqrUnk->printTextSub1(304);

		flowPtr = hqrUnk->printTextSub2(currentActorPtr->FRAME);

		if(!flowPtr) {
			currentActorPtr->ID = -1;
			return(-1);
		}

		currentActorPtr->ANIM = mode;

		*(int16 *)flowPtr = localSpecialTable[g_fitd->randRange(0, 3)]; // type ? color ?
		flowPtr += 2;
		*(int16 *)flowPtr = 30; // num of points
		flowPtr += 2;


		for(j = 0; j < 30; j++) {
			*(int16 *)flowPtr = g_fitd->randRange(actorZvPtr->ZVX1, actorZvPtr->ZVX2); //X
			flowPtr += 2;
			*(int16 *)flowPtr = g_fitd->randRange(actorZvPtr->ZVY1, actorZvPtr->ZVY2); //Y
			flowPtr += 2;
			*(int16 *)flowPtr = g_fitd->randRange(actorZvPtr->ZVZ1, actorZvPtr->ZVZ2); //Z
			flowPtr += 2;
		}

		for(j = 0; j < 30; j++) {
			*(int16 *)flowPtr = g_fitd->randRange(150, 300); // ?
			flowPtr += 2;
			*(int16 *)flowPtr = g_fitd->randRange(30, 80); // ?
			flowPtr += 2;
		}

		actorZvPtr->ZVX1 = X - 10;
		actorZvPtr->ZVX2 = X + 10;
		actorZvPtr->ZVY1 = Y;
		actorZvPtr->ZVY2 = Y - 1;
		actorZvPtr->ZVZ1 = Z - 10;
		actorZvPtr->ZVZ2 = Z + 10;

		break;
	}
	default: 
	{
		I_Warning("Unsupported case %d in createFlow\n", mode);
	}
	}

	actorTurnedToObj = 1;
	return(i);
}

void getHardClip() 
{
	ZVStruct *zvPtr = &currentProcessedActorPtr->zv;
	char *etageData = (char *)getRoomData(currentProcessedActorPtr->room);
	int16 numEntry;
	int i;

	etageData += *(int16 *)etageData;

	numEntry = *(int16 *)etageData;
	etageData += 2;

	for(i = 0; i < numEntry; i++) {
		ZVStruct zvCol;

		zvCol.ZVX1 = (int16)EPI_LE_U16(etageData + 0x00);
		zvCol.ZVX2 = (int16)EPI_LE_U16(etageData + 0x02);
		zvCol.ZVY1 = (int16)EPI_LE_U16(etageData + 0x04);
		zvCol.ZVY2 = (int16)EPI_LE_U16(etageData + 0x06);
		zvCol.ZVZ1 = (int16)EPI_LE_U16(etageData + 0x08);
		zvCol.ZVZ2 = (int16)EPI_LE_U16(etageData + 0x0A);

		if(checkZvCollision(zvPtr, &zvCol)) 
		{
			hardClip.ZVX1 = zvCol.ZVX1;
			hardClip.ZVX2 = zvCol.ZVX2;
			hardClip.ZVY1 = zvCol.ZVY1;
			hardClip.ZVY2 = zvCol.ZVY2;
			hardClip.ZVZ1 = zvCol.ZVZ1;
			hardClip.ZVZ2 = zvCol.ZVZ2;

			return;
		}

		etageData += 16;
	}

	hardClip.ZVX1 = 32000;
	hardClip.ZVX2 = -32000;
	hardClip.ZVY1 = 32000;
	hardClip.ZVY2 = -32000;
	hardClip.ZVZ1 = 32000;
	hardClip.ZVZ2 = -32000;
}

void animMove(int a, int b, int c, int d, int e, int f, int g) 
{
	if(currentProcessedActorPtr->speed == 4) 
	{
		anim(b, 1, -1);
	}
	if(currentProcessedActorPtr->speed == 5) 
	{
		anim(c, 1, -1);
	}
	if(currentProcessedActorPtr->speed == -1) 
	{
		if(currentProcessedActorPtr->ANIM == b) 
		{
			anim(a, 0, e);
		} else if(currentProcessedActorPtr->ANIM == c) 
		{
			anim(d, 0, a);
		} else 
		{
			anim(e, 1, -1); // walk backward
		}
	}
	if(currentProcessedActorPtr->speed == 0) 
	{
		if(currentProcessedActorPtr->ANIM == c) 
		{
			anim(d, 0, a);
		}
		else {
			if(currentProcessedActorPtr->rotateTarget == 0)	
			{
				anim(a, 1, -1);
			}
			if(currentProcessedActorPtr->rotateTarget == 1)	
			{
				anim(g, 0, a);
			}
			if(currentProcessedActorPtr->rotateTarget == -1) 
			{
				anim(f, 0, a);
			}
		}
	}
}

void setStage(int newStage, int newRoomLocal, int X, int Y, int Z) 
{
	int animX;
	int animY;
	int animZ;

	currentProcessedActorPtr->stage = newStage;
	currentProcessedActorPtr->room = newRoomLocal;

	if(g_fitd->getGameType() != GType_AITD1) 
	{
		currentProcessedActorPtr->hardMat = -1;
	}

	animX = currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX;
	animY = currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY;
	animZ = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ;

	currentProcessedActorPtr->zv.ZVX1 += X - animX;
	currentProcessedActorPtr->zv.ZVX2 += X - animX;

	currentProcessedActorPtr->zv.ZVY1 += Y - animY;
	currentProcessedActorPtr->zv.ZVY2 += Y - animY;

	currentProcessedActorPtr->zv.ZVZ1 += Z - animZ;
	currentProcessedActorPtr->zv.ZVZ2 += Z - animZ;

	currentProcessedActorPtr->roomX = X;
	currentProcessedActorPtr->roomY = Y;
	currentProcessedActorPtr->roomZ = Z;

	currentProcessedActorPtr->worldX = X;
	currentProcessedActorPtr->worldY = Y;
	currentProcessedActorPtr->worldZ = Z;

	currentProcessedActorPtr->modX = 0;
	currentProcessedActorPtr->modY = 0;
	currentProcessedActorPtr->modZ = 0;

	if(currentActorCameraTarget == currentProcessedActorIdx) 
	{
		if(newStage != currentEtage) 
		{
			changeFloor = 1;
			newFloor = newStage;
			newRoom = newRoomLocal;
		} else 
		{
			if(currentDisplayedRoom != newRoomLocal) 
			{
				needChangeRoom = 1;
				newRoom = newRoomLocal;
			}
		}
	} else 
	{
		if(currentDisplayedRoom != newRoomLocal) 
		{
			currentProcessedActorPtr->worldX -= (int16)((roomDataTable[currentDisplayedRoom].worldX - roomDataTable[newRoomLocal].worldX) * 10);
			currentProcessedActorPtr->worldY += (int16)((roomDataTable[currentDisplayedRoom].worldY - roomDataTable[newRoomLocal].worldY) * 10);
			currentProcessedActorPtr->worldZ += (int16)((roomDataTable[currentDisplayedRoom].worldZ - roomDataTable[newRoomLocal].worldZ) * 10);
		}

		//    objModifFlag1 = 1;
	}
}

void setupRealZv(ZVStruct *zvPtr) 
{
	int i;
	int16 *ptr = pointBuffer;

	zvPtr->ZVX1 = 32000;
	zvPtr->ZVY1 = 32000;
	zvPtr->ZVZ1 = 32000;
	zvPtr->ZVX2 = -32000;
	zvPtr->ZVY2 = -32000;
	zvPtr->ZVZ2 = -32000;

	for(i = 0; i < numOfPoints; i++) 
	{
		if(zvPtr->ZVX1 > (*ptr)) 
		{
			zvPtr->ZVX1 = *(ptr);
		} else {
			if(zvPtr->ZVX2 < (*ptr)) 
			{
				zvPtr->ZVX2 = *(ptr);
			}
		}
		ptr++;

		if(zvPtr->ZVY1 > (*ptr)) 
		{
			zvPtr->ZVY1 = *(ptr);
		} else 
		{
			if(zvPtr->ZVY2 < (*ptr)) 
			{
				zvPtr->ZVY2 = *(ptr);
			}
		}
		ptr++;

		if(zvPtr->ZVZ1 > (*ptr)) 
		{
			zvPtr->ZVZ1 = *(ptr);
		} else 
		{
			if(zvPtr->ZVZ2 < (*ptr)) 
			{
				zvPtr->ZVZ2 = *(ptr);
			}
		}
		ptr++;

	}
}

void doRealZv(actorStruct *actorPtr) 
{
	ZVStruct *zvPtr;

	computeScreenBox(0, 0, 0, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, listBody->get(actorPtr->bodyNum));

	zvPtr = &actorPtr->zv;

	setupRealZv(zvPtr);

	zvPtr->ZVX1 += actorPtr->roomX;
	zvPtr->ZVX2 += actorPtr->roomX;
	zvPtr->ZVY1 += actorPtr->roomY;
	zvPtr->ZVY2 += actorPtr->roomY;
	zvPtr->ZVZ1 += actorPtr->roomZ;
	zvPtr->ZVZ2 += actorPtr->roomZ;
}

void processLife(int lifeNum) 
{
	int exitLife = 0;
	//int switchVal = 0;
	int var_6;
	int switchVal = 0;

	currentLifeActorIdx = currentProcessedActorIdx;
	currentLifeActorPtr = currentProcessedActorPtr;
	currentLifeNum = lifeNum;

	currentLifePtr = listLife->get(lifeNum);

	while(!exitLife) {
		int lifeTempVar1;
		int lifeTempVar2;
		int lifeTempVar3;
		int lifeTempVar4;
		int lifeTempVar5;
		int lifeTempVar6;
		int lifeTempVar7;
		int16 currentOpcode;

		var_6 = -1;

		currentOpcode = *(int16 *)(currentLifePtr);
		currentLifePtr += 2;

		I_Warning("%d:opcode: %04X\n", lifeNum, currentOpcode);

		if(currentOpcode & 0x8000) {
			var_6 = *(int16 *)(currentLifePtr);
			currentLifePtr += 2;

			if(var_6 == -1) {
				I_Error("Unsupported newVar = -1\n");
			} else {
				currentProcessedActorIdx = objectTable[var_6].ownerIdx;

				if(currentProcessedActorIdx != -1) {
					currentProcessedActorPtr = &actorTable[currentProcessedActorIdx];

					goto processOpcode;
				} else {
					int opcodeLocated;

					if(g_fitd->getGameType() == GType_AITD1) {
						opcodeLocated = AITD1LifeMacroTable[currentOpcode & 0x7FFF];
					} else {
						opcodeLocated = AITD2LifeMacroTable[currentOpcode & 0x7FFF];
					}

					switch(opcodeLocated) {
						////////////////////////////////////////////////////////////////////////
					case LM_BODY: {
						objectTable[var_6].body = evalVar();
						break;
					}
					case LM_BODY_RESET: {
						objectTable[var_6].body = evalVar();
						objectTable[var_6].anim = evalVar();
						break;
					}
					case LM_TYPE: {
						lifeTempVar1 = (*(int16 *)(currentLifePtr)) & TYPE_MASK;
						currentLifePtr += 2;

						lifeTempVar2 = objectTable[var_6].flags;

						objectTable[var_6].flags = (objectTable[var_6].flags & (~TYPE_MASK)) + lifeTempVar1;
						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_ANIM_ONCE: {
						objectTable[var_6].anim = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animInfo = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animType = ANIM_ONCE;
						if(g_fitd->getGameType() >= GType_JACK)
							objectTable[var_6].frame = 0;
						break;
					}
					case LM_ANIM_REPEAT: {
						objectTable[var_6].anim = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animInfo = -1;
						objectTable[var_6].animType = ANIM_REPEAT;
						if(g_fitd->getGameType() >= GType_JACK)
							objectTable[var_6].frame = 0;
						break;
					}
					case LM_ANIM_ALL_ONCE: {
						objectTable[var_6].anim = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animInfo = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animType = ANIM_ONCE | ANIM_UNINTERRUPTABLE;
						if(g_fitd->getGameType() >= GType_JACK)
							objectTable[var_6].frame = 0;
						break;
					}
					case    LM_ANIM_RESET: 
					{
						SYS_ASSERT(g_fitd->getGameType() >= GType_JACK);
						objectTable[var_6].anim = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animInfo = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						objectTable[var_6].animType = ANIM_ONCE | ANIM_RESET ;
						objectTable[var_6].frame = 0 ;
						break ;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_MOVE: { // MOVE
						objectTable[var_6].stage = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].room = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].lifeMode = 0;
						break;
					}
					case LM_ANGLE: {
						objectTable[var_6].alpha = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].beta = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].gamma = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						break;
					}
					case LM_STAGE: { // stage
						objectTable[var_6].stage = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].room = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].x = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].y = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						objectTable[var_6].z = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						//objModifFlag1 = 1;

						break;
					}
					case LM_TEST_COL: {
						if(*(int16 *)(currentLifePtr)) {
							objectTable[var_6].flags |= 0x20;
						} else {
							objectTable[var_6].flags &= 0xFFDF;
						}

						currentLifePtr += 2;

						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_LIFE: {
						objectTable[var_6].life = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
						break;
					}
					case LM_LIFE_MODE: { // LIFE_MODE
						lifeTempVar1 = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;


						if(lifeTempVar1 != objectTable[var_6].lifeMode) {
							objectTable[var_6].lifeMode = lifeTempVar1;
							//objModifFlag1 = 1;
						}
						break;
					}
					case LM_FOUND_NAME: { // FOUND_NAME
						objectTable[var_6].foundName = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						break;
					}
					case LM_FOUND_BODY: { // FOUND_BODY
						objectTable[var_6].foundBody = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						break;
					}
					case LM_FOUND_FLAG: { // FOUND_FLAG
						objectTable[var_6].flags2 &= 0xE000;
						objectTable[var_6].flags2 |= *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						break;
					}
					case LM_FOUND_WEIGHT: {
						objectTable[var_6].positionInTrack = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;

						break;
					}
					case  LM_START_CHRONO: { // arrive in original interpreter too
						break;
					}
					////////////////////////////////////////////////////////////////////////
					default: {
						I_Error("Unsupported opcode %X when actor isn't in floor\n", currentOpcode & 0x7FFF);
						break;
					}
					}
				}
			}
		} else {
			int opcodeLocated;
processOpcode:

			if(g_fitd->getGameType() == GType_AITD1) {
				opcodeLocated = AITD1LifeMacroTable[currentOpcode & 0x7FFF];
			} else {
				opcodeLocated = AITD2LifeMacroTable[currentOpcode & 0x7FFF];
			}

			switch(opcodeLocated) {
			case LM_BODY: {
				lifeTempVar1 = evalVar();

				objectTable[currentProcessedActorPtr->ID].body = lifeTempVar1;

				if(currentProcessedActorPtr->bodyNum != lifeTempVar1) {
					currentProcessedActorPtr->bodyNum = lifeTempVar1;

					if(currentProcessedActorPtr->flags & 1) {
						if((currentProcessedActorPtr->ANIM != -1) && (currentProcessedActorPtr->bodyNum != -1)) {
							char *pAnim = listAnim->get(currentProcessedActorPtr->ANIM);
							char *pBody;

							if(g_fitd->getGameType() >= GType_JACK) {
								/*                  if (bFlagDecal)
								 gereDecal(); */
							}

							pBody = listBody->get(currentProcessedActorPtr->bodyNum);

							/*    if(g_fitd->getGameType() >= GType_JACK)
							 {
							 setInterAnimObject2(currentProcessedActorPtr->FRAME, pAnim, pBody, TRUE, Objet->AnimDecal);
							 }
							 else */
							{
								setInterAnimObjet(currentProcessedActorPtr->FRAME, pAnim, pBody);
							}
						}
					} else {
						mainVar1 = 1;
					}
				}
				break;
			}
			case LM_BODY_RESET: {
				int param1;
				int param2;

				param1 = evalVar();
				param2 = evalVar();

				objectTable[currentProcessedActorPtr->ID].body = param1;
				objectTable[currentProcessedActorPtr->ID].anim = param2;

				currentProcessedActorPtr->bodyNum = param1;

				if(currentProcessedActorPtr->flags & 1) {
					char *pAnim = listAnim->get(currentProcessedActorPtr->ANIM);
					char *pBody;

					if(g_fitd->getGameType() >= GType_JACK) {
						/*                  if (bFlagDecal)
						 gereDecal(); */
					}

					pBody = listBody->get(currentProcessedActorPtr->bodyNum);

					initAnimInBody(0, pAnim, pBody);
					anim(param2, 4, -1);
				} else {
					mainVar1 = 1;
				}
				break;
			}
			case LM_DO_REAL_ZV: {
				doRealZv(currentProcessedActorPtr);
				break;
			}
			case LM_DEF_ZV: { // DEF_ZV
				currentProcessedActorPtr->zv.ZVX1 = currentProcessedActorPtr->roomX + *(int16 *)currentLifePtr + currentProcessedActorPtr->modX;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVX2 = currentProcessedActorPtr->roomX + *(int16 *)currentLifePtr + currentProcessedActorPtr->modX;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVY1 = currentProcessedActorPtr->roomY + *(int16 *)currentLifePtr + currentProcessedActorPtr->modY;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVY2 = currentProcessedActorPtr->roomY + *(int16 *)currentLifePtr + currentProcessedActorPtr->modY;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVZ1 = currentProcessedActorPtr->roomZ + *(int16 *)currentLifePtr + currentProcessedActorPtr->modZ;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVZ2 = currentProcessedActorPtr->roomZ + *(int16 *)currentLifePtr + currentProcessedActorPtr->modZ;
				currentLifePtr += 2;

				break;
			}
			case LM_DEF_ABS_ZV: {
				currentProcessedActorPtr->zv.ZVX1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVX2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVY1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVY2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVZ1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVZ2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_DO_ROT_ZV: { // DO_ROT_ZV
				getZvRot(listBody->get(currentProcessedActorPtr->bodyNum), &currentProcessedActorPtr->zv,
				         currentProcessedActorPtr->alpha,
				         currentProcessedActorPtr->beta,
				         currentProcessedActorPtr->gamma);

				currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ;
				currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_DO_CARRE_ZV: { // DO_CARRE_ZV
				getZvCube(listBody->get(currentProcessedActorPtr->bodyNum), &currentProcessedActorPtr->zv);

				currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ;
				currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_TYPE: { // TYPE
				lifeTempVar1 = *(int16 *)(currentLifePtr) & 0x1D1;
				currentLifePtr += 2;

				lifeTempVar2 = currentProcessedActorPtr->flags;

				currentProcessedActorPtr->flags = (currentProcessedActorPtr->flags & 0xFE2E) + lifeTempVar1;

				if(lifeTempVar2 & 1) {
					if(!(lifeTempVar1 & 1)) {
						stopAnim(currentProcessedActorIdx);
					}
				}

				if(lifeTempVar1 & 1) {
					if(!(lifeTempVar2 & 8)) {
						deleteSub(currentProcessedActorIdx);
					}
				}

				break;
			}
			case LM_GET_HARD_CLIP: { //GET_HARD_CLIP
				getHardClip();
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_ANIM_ONCE: {
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if(lifeTempVar1 == -1) {
					currentProcessedActorPtr->ANIM = -1;
					currentProcessedActorPtr->nextAnim = -2;
				} else {
					anim(lifeTempVar1, 0, lifeTempVar2);
				}

				break;
			}
			case LM_ANIM_REPEAT: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				anim(lifeTempVar1, 1, -1);

				break;
			}
			case LM_ANIM_ALL_ONCE: {
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				anim(lifeTempVar1, 2, lifeTempVar2);

				break;
			}
			case LM_ANIM_RESET: {
				int param1;
				int param2;

				param1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				param2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(param1 == -1) {
					currentProcessedActorPtr->ANIM = -1;
					currentProcessedActorPtr->nextAnim = -2;
				} else {
					anim(param1, 4, param2);
				}

				break;
			}
			case LM_ANIM_HYBRIDE_ONCE: {
				// TODO
				currentLifePtr += 4;
				break;
			}
			case LM_ANIM_HYBRIDE_REPEAT: {
				// TODO
				currentLifePtr += 4;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_HIT: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar5 = evalVar();
				lifeTempVar6 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				hit(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6);

				break;
			}
			case LM_FIRE: { // FIRE
				if(g_fitd->getGameType() == GType_AITD1) {
					int fireAnim;
					int shootFrame;
					int emitPoint;
					int zvSize;
					int hitForce;
					int nextAnim;

					fireAnim = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					shootFrame = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					emitPoint = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					zvSize = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					hitForce = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					nextAnim = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;

					fire(fireAnim, shootFrame, emitPoint, zvSize, hitForce, nextAnim);
				} else { // use an emitter model
					int fireAnim;
					int shootFrame;
					int emitPoint;
					int emitModel;
					int zvSize;
					int hitForce;
					int nextAnim;

					fireAnim = evalVar();
					shootFrame = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					emitPoint = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					emitModel = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					zvSize = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					hitForce = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
					nextAnim = evalVar();

					fire(fireAnim, shootFrame, emitPoint, zvSize, hitForce, nextAnim); // todo: implement emit model
				}

				break;
			}
			case LM_FIRE_UP_DOWN: { // TODO AITD3 only
				evalVar();
				currentLifePtr += 12;
				evalVar();
				break;
			}
			case LM_HIT_OBJECT: { // HIT_OBJECT
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				currentProcessedActorPtr->animActionType = 8;
				currentProcessedActorPtr->animActionParam = lifeTempVar1;
				currentProcessedActorPtr->hitForce = lifeTempVar2;
				currentProcessedActorPtr->animActionHotpoint = -1;
				break;
			}
			case LM_STOP_HIT_OBJECT: { // cancel hit obj
				if(currentProcessedActorPtr->animActionType == 8) {
					currentProcessedActorPtr->animActionType = 0;
					currentProcessedActorPtr->animActionParam = 0;
					currentProcessedActorPtr->hitForce = 0;
					currentProcessedActorPtr->animActionHotpoint = -1;
				}

				break;
			}
			case LM_THROW: { // throw
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar6 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar7 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				throwObj(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6, lifeTempVar7);

				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_MOVE: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				setMoveMode(lifeTempVar1, lifeTempVar2);

				break;
			}
			case LM_RESET_MOVE_MANUAL: {
				resetRotateParam();
				break;
			}
			case LM_CONTINUE_TRACK: {
				char *ptr;

				ptr = listTrack->get(currentProcessedActorPtr->trackNumber);

				ptr += currentProcessedActorPtr->positionInTrack * 2;

				if(*(int16 *)ptr == 5) {
					currentProcessedActorPtr->positionInTrack++;
				}
				break;
			}
			case LM_DO_MOVE: {
				if(g_fitd->getGameType() == GType_AITD1) {
					processTrack();
				} else {
					processTrack2();
				}
				break;
			}
			case LM_ANIM_MOVE: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar6 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar7 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				animMove(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6, lifeTempVar7);

				break;
			}
			case LM_MANUAL_ROT: { // MANUAL_ROT
				if(g_fitd->getGameType() == GType_AITD1) {
					manualRot(240);
				} else {
					manualRot(90);
				}
				break;
			}
			case LM_SET_BETA: { // SET_BETA
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(currentProcessedActorPtr->beta != lifeTempVar1) {
					if(currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->rotate.newAngle != lifeTempVar1) {
						startActorRotation(currentProcessedActorPtr->beta, lifeTempVar1, lifeTempVar2, &currentProcessedActorPtr->rotate);
					}

					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				}

				break;
			}
			case LM_SET_ALPHA: { // SET_ALPHA
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(currentProcessedActorPtr->alpha != lifeTempVar1) {
					if(currentProcessedActorPtr->rotate.param == 0 || lifeTempVar1 != currentProcessedActorPtr->rotate.newAngle) {
						startActorRotation(currentProcessedActorPtr->alpha, lifeTempVar1, lifeTempVar2, &currentProcessedActorPtr->rotate);
					}

					currentProcessedActorPtr->alpha = updateActorRotation(&currentProcessedActorPtr->rotate);
				}

				break;
			}
			case LM_ANGLE: { // ANGLE
				currentProcessedActorPtr->alpha = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->beta = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->gamma = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_COPY_ANGLE: {
				// TODO !
				currentLifePtr += 2;
				break;
			}
			case LM_STAGE: { // STAGE
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				setStage(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5);

				break;
			}
			case LM_TEST_COL: { // TEST_COL
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(lifeTempVar1) {
					currentProcessedActorPtr->dynFlags |= 1;
				} else {
					currentProcessedActorPtr->dynFlags &= 0xFFFE;
				}

				break;
			}
			case LM_UP_COOR_Y: { // UP_COOR_Y
				startActorRotation(0, -2000, -1, &currentProcessedActorPtr->fall);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_LIFE: { // LIFE
				currentProcessedActorPtr->life = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				break;
			}
			case LM_STAGE_LIFE: {
				objectTable[currentProcessedActorPtr->ID].field_24 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				break;
			}
			case LM_LIFE_MODE: { // LIFE_MODE
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(g_fitd->getGameType() == GType_AITD1) {
					lifeTempVar2 = currentProcessedActorPtr->lifeMode;
				} else {
					lifeTempVar2 = currentProcessedActorPtr->lifeMode & 3;
				}

				if(lifeTempVar1 != lifeTempVar2) {
					currentProcessedActorPtr->lifeMode = lifeTempVar1;
					//objModifFlag1 = 1;
				}
				break;
			}
			case LM_DELETE: { // DELETE
				if(g_fitd->getGameType() == GType_AITD1) {
					lifeTempVar1 = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
				} else {
					lifeTempVar1 = evalVar();
				}

				deleteObject(lifeTempVar1);

				if(objectTable[lifeTempVar1].foundBody != -1) {
					if(g_fitd->getGameType() == GType_AITD1) { // TODO: check, really usefull ?
						objectTable[lifeTempVar1].flags2 &= 0x7FFF;
					}
					objectTable[lifeTempVar1].flags2 |= 0x4000;
				}

				break;
			}
			case LM_SPECIAL: { // SPECIAL
				lifeTempVar1 = *(int16 *)(currentLifePtr) & 0x1D1;
				currentLifePtr += 2;

				switch(lifeTempVar1) {
				case 0: {
					createFlow(0,
					           currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX,
					           currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY,
					           currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ,
					           currentProcessedActorPtr->stage,
					           currentProcessedActorPtr->room,
					           currentProcessedActorPtr->alpha,
					           currentProcessedActorPtr->beta,
					           currentProcessedActorPtr->gamma,
					           &currentProcessedActorPtr->zv);
					break;
				}
				case 1: {
					currentProcessedActorPtr = &actorTable[currentProcessedActorPtr->HIT_BY];

					createFlow(1,
					           currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX + currentProcessedActorPtr->hotPoint.x,
					           currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY + currentProcessedActorPtr->hotPoint.y,
					           currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ + currentProcessedActorPtr->hotPoint.z,
					           currentProcessedActorPtr->stage,
					           currentProcessedActorPtr->room,
					           0,
					           -currentProcessedActorPtr->beta,
					           0,
					           NULL);

					currentProcessedActorPtr = currentLifeActorPtr;


					break;
				}
				case 4: {
					createFlow(4,
					           currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX,
					           currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY,
					           currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ,
					           currentProcessedActorPtr->stage,
					           currentProcessedActorPtr->room,
					           currentProcessedActorPtr->alpha,
					           currentProcessedActorPtr->beta,
					           currentProcessedActorPtr->gamma,
					           &currentProcessedActorPtr->zv);
					break;
				}
				}
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_START_CHRONO: { //START_CHRONO
				startChrono(&currentProcessedActorPtr->CHRONO);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_FOUND: { // FOUND
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				foundObject(lifeTempVar1, 1);

				break;
			}
			case LM_TAKE: { // TAKE
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				take(lifeTempVar1);

				break;
			}
			case LM_IN_HAND: { // IN_HAND
				if(g_fitd->getGameType() == GType_AITD1) {
					inHand = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;
				} else {
					if(g_fitd->getGameType() == GType_JACK) {
						inHandTable[currentInHand] = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
					} else {
						inHandTable[currentInHand] = evalVar();
					}
				}
				break;
			}
			case LM_DROP: { // DROP
				lifeTempVar1 = evalVar();
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				I_Warning("Drop\n");
				// drop(lifeTempVar1, lifeTempVar2);

				break;
			}
			case LM_PUT: {
				int x;
				int y;
				int z;
				int room;
				int stage;
				int alpha;
				int beta;
				int gamma;
				int idx;

				idx = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				x = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				y = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				z = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				room = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				stage = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				alpha = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				beta = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				gamma = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				put(x, y, z, room, stage, alpha, beta, gamma, idx);

				break;
			}
			case LM_PUT_AT: { // PUT_AT
				int objIdx1;
				int objIdx2;

				objIdx1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				objIdx2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				putAt(objIdx1, objIdx2);
				break;
			}
			case LM_FOUND_NAME: { // FOUND_NAME
				objectTable[currentProcessedActorPtr->ID].foundName = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				break;
			}
			case LM_FOUND_BODY: { // FOUND_BODY
				objectTable[currentProcessedActorPtr->ID].foundBody = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				break;
			}
			case LM_FOUND_FLAG: { // FOUND_FLAG
				objectTable[currentProcessedActorPtr->ID].flags2 &= 0xE000;
				objectTable[currentProcessedActorPtr->ID].flags2 |= *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				break;
			}
			case LM_FOUND_WEIGHT: { // FOUND_WEIGHT
				objectTable[currentProcessedActorPtr->ID].positionInTrack = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				break;
			}
			case LM_FOUND_LIFE: { // FOUND_LIFE
				objectTable[currentProcessedActorPtr->ID].foundLife = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				break;
			}
			case LM_READ: { // READ
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				fadeOut(0x20, 0);

				I_Warning("ReadBook\n");
				//          readBook(lifeTempVar2+1, lifeTempVar1);

				if(g_fitd->getGameType() == GType_AITD1) {
					fadeOut(4, 0);
				}

				mainVar1 = 2;

				break;
			}
			case LM_READ_ON_PICTURE: { // TODO
				currentLifePtr += 16;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_ANIM_SAMPLE: { // ANIM_SAMPLE
				lifeTempVar1 = evalVar();

				lifeTempVar2 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(currentProcessedActorPtr->END_FRAME != 0) {
					if(currentProcessedActorPtr->ANIM == lifeTempVar2) {
						if(currentProcessedActorPtr->FRAME == lifeTempVar3) {
							playSound(lifeTempVar1);
							//setSampleFreq(0);
						}
					}
				}
				break;
			}
			case    LM_2D_ANIM_SAMPLE: {
				int sampleNumber;
				int animNumber;
				int frameNumber;

				sampleNumber = evalVar();
				animNumber = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				frameNumber = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				break; // TODO: implement
			}
			case LM_SAMPLE: {
				int sampleNumber;

				if(g_fitd->getGameType() == GType_AITD1) {
					sampleNumber = evalVar();
				} else {
					if(g_fitd->getGameType() == GType_JACK) {
						sampleNumber = evalVar();
					} else {
						sampleNumber = *(int16 *)(currentLifePtr);
						currentLifePtr += 2;
					}
				}

				playSound(sampleNumber);
				//setSampleFreq(0);
				break;
			}
			case LM_REP_SAMPLE: { // sample TODO!
				if(g_fitd->getGameType() == GType_AITD1) {
					evalVar();
					currentLifePtr += 2;
				} else {
					currentLifePtr += 4;
				}
				break;
			}
			case LM_STOP_SAMPLE: { // todo
				break ;
			}
			case LM_SAMPLE_THEN: { //todo
				if(g_fitd->getGameType() == GType_AITD1) {
					playSound(evalVar());
					genVar7 = evalVar();
				} else {
					int newSample;

					if(g_fitd->getGameType() == GType_JACK) {
						newSample = evalVar();
						genVar7 = evalVar();
					} else {
						newSample = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						genVar7 = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
					}

					playSound(newSample);
				}
				// setSampleFreq(0);
				break;
			}
			case LM_SAMPLE_THEN_REPEAT: { //todo
				playSound(evalVar());
				genVar7 = evalVar() | 0x4000;
				// setSampleFreq(0);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_MUSIC: { // MUSIC
				int newMusicIdx = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				playMusic(newMusicIdx);
				break;
			}
			case LM_NEXT_MUSIC: { // TODO
				int musicIdx = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(currentMusic == -1) {
					playMusic(musicIdx);
				} else {
					nextMusic = musicIdx;
				}

				break;
			}
			case LM_FADE_MUSIC: { // ? fade out music and play another music ?
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(currentMusic != -1) {
					fadeMusic(0, 0, 0x8000);  // fade out music
					startChrono(&musicChrono); // fade out music timer
					currentMusic = -2;         // waiting next music
					nextMusic = lifeTempVar1;    // next music to play
				} else {
					playMusic(lifeTempVar1);
				}

				break;
			}
			case LM_RND_FREQ: { // TODO
				currentLifePtr += 2;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_LIGHT: { // LIGHT
				lifeTempVar1 = 2 - ((*(int16 *)(currentLifePtr)) << 1);
				currentLifePtr += 2;

				if(g_fitd->getGameType() >= GType_JACK || (!CVars[getCVarsIdx(KILLED_SORCERER)])) {
					if(lightVar1 != lifeTempVar1) {
						lightVar1 = lifeTempVar1;
						lightVar2 = 1;
					}
				}

				break;
			}
			case LM_SHAKING: { // SHAKING
				I_Warning("Shaking\n");
				//shakingState = shakingAmplitude = *(short int*)(currentLifePtr);
				currentLifePtr += 2;

				/*          if(shakingState==0)
				 {
				 stopShaking();
				 } */
				break;
			}
			case LM_PLUIE: {
				// TODO
				break;
			}
			case LM_WATER: { // ? shaking related
				// TODO: Warning, AITD1/AITD2 diff
				I_Warning("Shaking related\n");
				//          mainLoopVar1 = shakeVar1 = *(short int*)(currentLifePtr);
				currentLifePtr += 2;

				/*          if(mainLoopVar1)
				 {
				 //setupShaking(-600);
				 }
				 else
				 {
				 //setupShaking(1000);
				 } */

				break;
			}
			case LM_CAMERA_TARGET: { // CAMERA_TARGET
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(lifeTempVar1 != currentCameraTarget) { // same stage
					lifeTempVar2 = objectTable[lifeTempVar1].ownerIdx;

					if(lifeTempVar2 != -1) {
						if(g_fitd->getGameType() == GType_AITD1) {
							currentCameraTarget = lifeTempVar1;
							currentActorCameraTarget = lifeTempVar2;

							lifeTempVar3 = actorTable[currentActorCameraTarget].room;

							if(lifeTempVar3 != currentDisplayedRoom) {
								needChangeRoom = 1;
								newRoom = lifeTempVar3;
							}
						} else {
							// security case, the target actor may be still be in list while already changed of stage
							// TODO: check if AITD1 could use the same code (quite probable as it's only security)
							if(objectTable[lifeTempVar1].stage != currentEtage) {
								changeFloor = 1;
								newFloor = actorTable[lifeTempVar2].stage;
								newRoom = actorTable[lifeTempVar2].room;
							} else {
								currentCameraTarget = lifeTempVar1;
								currentActorCameraTarget = lifeTempVar2;

								lifeTempVar3 = actorTable[currentActorCameraTarget].room;

								if(lifeTempVar3 != currentDisplayedRoom) {
									needChangeRoom = 1;
									newRoom = lifeTempVar3;
								}
							}
						}
					} else { // different stage
						if(objectTable[lifeTempVar1].stage != currentEtage) {
							changeFloor = 1;
							newFloor = objectTable[lifeTempVar1].stage;
							newRoom = objectTable[lifeTempVar1].room;
						} else {
							if(currentDisplayedRoom != objectTable[lifeTempVar1].room) {
								needChangeRoom = 1;
								newRoom = objectTable[lifeTempVar1].room;
							}
						}
					}
				}

				break;
			}
			case LM_PICTURE: { // displayScreen
				if(g_fitd->getGameType() == GType_AITD1) {
					unsigned int chrono;

					loadPakToPtr("ITD_RESS", *(int16 *)currentLifePtr, aux);
					currentLifePtr += 2;

					copyToScreen(aux, unkScreenVar);
					flip();

					startChrono(&chrono);
					lifeTempVar1 = *(int16 *)currentLifePtr;
					currentLifePtr += 2;

					playSound(*(int16 *)currentLifePtr);
					currentLifePtr += 2;

					//soundFunc(0);

					do {
						unsigned int time;
						process_events();
						readKeyboard();

						timeGlobal++;
						g_fitd->setTimer(timeGlobal);

						time = evalChrono(&chrono);

						if(time > (unsigned int)lifeTempVar1)
							break;
					} while(!input2 && !input1);

					unfreezeTime();

					mainVar1 = 1;

					break;
				} else {
					unsigned int chrono;
					unsigned char lpalette[0x300];
					int16 ressourceIdx;

					ressourceIdx = *(int16 *)(currentLifePtr);
					currentLifePtr += 2;

					loadPakToPtr("ITD_RESS", ressourceIdx, aux);

					copyPalette(aux + 64000, (char *)lpalette);

					if(g_fitd->getGameType() < GType_AITD3)
						convertPaletteIfRequired((unsigned char *)lpalette);

					fadeOut(0x10, 0);
					fadeIn(lpalette);

					g_driver->setPalette(lpalette);
					copyPalette((char *)lpalette, g_driver->_palette);

					copyToScreen(aux, screen);

					startChrono(&chrono);

					do {
						unsigned int time;
						process_events();
						readKeyboard();

						g_driver->CopyBlockPhys((unsigned char *)screen, 0, 0, 320, 200);
						g_driver->startFrame();
						flipScreen();

						timeGlobal++;
						g_fitd->setTimer(timeGlobal);

						time = evalChrono(&chrono);

						if(time > 100)
							break;
					} while(!input2 && !input1);

					unfreezeTime();

					currentLifePtr += 4;

					break;
				}
			}
			case LM_PLAY_SEQUENCE: { // sequence
				uint16 sequenceIdx;
				uint16 fadeEntry;
				uint16 fadeOut;

				freezeTime();

				sequenceIdx = *(uint16 *)(currentLifePtr);
				currentLifePtr += 2;

				fadeEntry = *(uint16 *)(currentLifePtr);
				currentLifePtr += 2;

				fadeOut = *(uint16 *)(currentLifePtr);
				currentLifePtr += 2;

				playSequence(sequenceIdx, fadeEntry, fadeOut);

				unfreezeTime();

				break;
			}
			case LM_DEF_SEQUENCE_SAMPLE: {
				uint16 numParams;
				int i;

				numParams = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				SYS_ASSERT(numParams <= NUM_MAX_SEQUENCE_PARAM);

				for(i = 0; i < numParams; i++) {
					sequenceParams[i].frame = EPI_LE_U16(currentLifePtr);
					currentLifePtr += 2;
					sequenceParams[i].sample = EPI_LE_U16(currentLifePtr);
					currentLifePtr += 2;
				}

				numSequenceParam = numParams;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_PROTECT: { // protection opcode
				//protection = 1;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_INVENTORY: { // INVENTORY
				statusScreenAllowed = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				break;
			}
			case LM_SET_INVENTORY: {
				int inventoryIndex = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				/*          if(indeventoyIndex != currentInHand)
				 {
				 if(currentInHand<2)
				 {
				 int i;

				 for(i=0;i<inventory[currentInHand];i++)
				 {
				 objectTable[inventoryTable[currentInHand][i]].flags2&=0x7FFF;
				 }

				 currentInHand = inventoryIndex
				 }
				 }*/
				break;
			}
			case LM_SET_GROUND: {
				groundLevel = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				break;
			}
			case LM_MESSAGE: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				makeMessage(lifeTempVar1);

				break;
			}
			case LM_MESSAGE_VALUE: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)(currentLifePtr); // unused param ?
				currentLifePtr += 2;

				makeMessage(lifeTempVar1);

				break;
			}
			case LM_END_SEQUENCE: { // ENDING
				// TODO!
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_VAR: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				vars[lifeTempVar1] = evalVar();
				break;
			}
			case LM_INC: { // INC_VAR
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				vars[lifeTempVar1]++;
				break;
			}
			case LM_DEC: { // DEC_VAR
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				vars[lifeTempVar1]--;
				break;
			}
			case LM_ADD: { // ADD_VAR
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				vars[lifeTempVar1] += evalVar();
				break;
			}
			case LM_SUB: { // SUB_VAR
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				vars[lifeTempVar1] -= evalVar();
				break;
			}
			case LM_MODIF_C_VAR:
			case LM_C_VAR: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				CVars[lifeTempVar1] = evalVar();
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_IF_EGAL: {
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if(lifeTempVar1 == lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_DIFFERENT: {
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if(lifeTempVar1 != lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_SUP_EGAL: {
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if(lifeTempVar1 >= lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_SUP: {
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if(lifeTempVar1 > lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_INF_EGAL: {
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if(lifeTempVar1 <= lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_INF: {
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if(lifeTempVar1 < lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_GOTO: {
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += lifeTempVar1 * 2;
				currentLifePtr += 2;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_SWITCH: { // SWITCH
				switchVal = evalVar();
				break;
			}
			case LM_CASE: { // CASE
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				if(lifeTempVar1 == switchVal) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_MULTI_CASE: { // MULTI_CASE
				int i;
				lifeTempVar1 = *(int16 *)(currentLifePtr);
				currentLifePtr += 2;

				lifeTempVar2 = 0;

				for(i = 0; i < lifeTempVar1; i++) {
					if(*(int16 *)(currentLifePtr) == switchVal) {
						lifeTempVar2 = 1;
					}
					currentLifePtr += 2;
				}

				if(!lifeTempVar2) {
					lifeTempVar2 = *(int16 *)(currentLifePtr);
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				} else {
					currentLifePtr += 2;
				}
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_RETURN: 
			{
				exitLife = 1;
				break;
			}
			case LM_END: 
			{
				exitLife = 1;
				break;
			}
			default: 
			{
				I_Error("Unknown opcode %X in processLife\n", currentOpcode & 0x7FFF);
			}
			}
		}

		if(var_6 != -1) 
		{
			currentProcessedActorIdx = currentLifeActorIdx;
			currentProcessedActorPtr = currentLifeActorPtr;
		}

	}

	currentLifeNum = -1;
}

} // end of namespace Fitd
