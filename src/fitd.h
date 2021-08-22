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

#ifndef FITD_FITD_H
#define FITD_FITD_H

#ifdef HAVE_CONFIG_H
#undef HAVE_CONFIG_H
#define UNIX
#include "common/endian.h"
#define HAVE_CONFIG_H
#else
#include "epi/types.h"
#include "epi/endianess.h"
#endif
#include "cvars.h"

namespace Fitd {

struct hqrEntryStruct;

enum gameType {
	GType_AITD1 = 1,
	GType_JACK = 2,
	GType_AITD2 = 3,
	GType_AITD3 = 4,
	GType_TIMEGATE = 5,
};

class FitdEngine {
	gameType _gameType;
	enumCVars *_currentCVarTable;
	uint32 _numCVars;

	void sysInit();
	void detectGame();
	
	uint32 _timer;

public:
	FitdEngine();
	~FitdEngine();
	void run();
	void startup();
	void preloadResource();
	uint32 getGameType();
	int getNumCVars();
	uint32 randRange(int min, int max);
	int32 getCVarsIdx(enumCVars searchedType);
	void delay(uint32 ms);
	int32 getTicks();
	uint32 getTimer() { return _timer; }
	void setTimer(uint32 value) { _timer = value; }

// Lists
	hqrEntryStruct *_listMus;
	hqrEntryStruct *_listSamp;
};

extern FitdEngine *g_fitd;

} // end of namespace Fitd

#endif
