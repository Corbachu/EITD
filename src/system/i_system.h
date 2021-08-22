//----------------------------------------------------------------------------
//  EDGE2 Platform Interface (EPI) Header
//----------------------------------------------------------------------------
//
//  Copyright (c) 1999-2009  The EDGE2 Team.
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
//----------------------------------------------------------------------------

#ifndef __I_SYSTEM_H__
#define __I_SYSTEM_H__
#include "../../epi/timestamp.h"

//--------------------------------------------------------
//  SYSTEM functions.
//--------------------------------------------------------

// -ACB- 1999/09/20 Removed system specific attribs.

void I_SetupSignalHandlers(bool allow_coredump);
void I_CheckAlreadyRunning(void);

void I_SystemStartup(void);
// This routine is responsible for getting things off the ground, in
// particular calling all the other platform initialisers (i.e.
// I_StartupControl, I_StartupGraphics, I_StartupMusic and
// I_StartupSound).  Does whatever else the platform code needs.

//--- void I_Loop(void);
// This is called by EDGE2 to begin the main engine loop, and is not
// expected to return.  It must call engine::Tick() to perform a
// single loop in the system, which processes events, updates the play
// simulation, keeps sound and music playing, and most importantly
// renders a single frame of graphics.

void I_Printf(const char *message,...) GCCATTR((format(printf, 1, 2)));
// The generic print function.  If in text mode, the message should be
// displayed on the text mode screen.  This function should also call
// I_Debugf() and CON_Printf().

//void I_Debugf(const char *message,...) GCCATTR((format(printf, 1, 2)));
// The generic debugging function.

void I_PutTitle(const char *title);
// This function should clear the text mode screen (or window), and
// write the given title in a banner at the top (in a stand-out way,
// e.g. bright white on a red background).

void I_Error(const char *error,...) GCCATTR((format(printf, 1, 2)));
// The error function.  All fatal errors call I_Error().  This
// I_CloseProgram).

void I_DisplayExitScreen(void);
// Displays the exit screen on the text mode screen (or window).  The
// text typically comes from the "ENDOOM" lump in the IWAD.

void I_SystemShutdown(void);
// The opposite of the I_SystemStartup routine.  This will shutdown
// everything running in the platform code, by calling the other
// termination functions (I_ShutdownSound, I_ShutdownMusic,
// I_ShutdownGraphics and I_ShutdownControl), and doing anything else
// the platform code needs to (e.g. freeing all other resources).

void I_CloseProgram(int exitnum) GCCATTR((noreturn));
// Exit the program immediately, using the given `exitnum' as the
// program's exit status.  This is the very last thing done, and
// I_SystemShutdown() is guaranteed to have already been called.

void I_TraceBack(void);
// Aborts the program, and displays a traceback if possible (which is
// useful for debugging).  The system code should check for the
// "-traceback" option, and when present call this routine instead of
// I_CloseProgram() whenever a fatal error occurs.

void I_Warning(const char *warning,...) GCCATTR((format(printf, 1, 2)));
// Writes a warning to the console and the debug file (if any).  This
// function should call CON_Printf().

//void I_Sleep(int millisecs);
// -AJA- 2005/01/21: sleep for the given number of milliseconds.

//void I_Tactile (int frequency, int intensity, int select);
// -CW- 2017/01/23 Force Feedback

void I_MessageBox(const char *message, const char *title);
// -AJA- 2007/04/13: display a system message box with the
// given message (typically a serious error message).

extern const char *win32_exe_path;


//void I_Logf(const char *message,...) GCCATTR((format(printf, 1, 2)));

//void I_GLf(const char *message, ...) GCCATTR((format(printf, 1, 2)));

void I_PrintGLSL(const char *message, ...) GCCATTR((format(printf, 1, 2)));

//#define L_WriteDebug  I_Debugf
//#define L_WriteLog    I_Logf
//#define L_WriteOpenGL I_GLf

#define OBJSET(obj, val) (memset(&(obj), (val), sizeof(obj)))
#define OBJZERO(obj)     OBJSET((obj), 0)


#endif /*__I_SYSTEM_H__*/

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
