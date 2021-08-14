//----------------------------------------------------------------------------
//  EDGE Win32 System Internal header ("formerly textconsole")
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

#include "i_sdlinc.h"
#include "../system/i_system.h"

#include "textconsole.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <winternl.h>

#define DUMMYUNIONNAMEN(n)
#include <windows.h>  // -AJA- added

#include <objbase.h>
#include <wchar.h>
#include <winbase.h>
#include <winnls.h>
#include <mmsystem.h>
#include <signal.h>
#include <windowsx.h> // -ACB- 2000/07/19 Cracker API

#include <sys/types.h> // Required for _stat()


#define INTOLERANT_MATH 1  // -AJA- FIXME: temp fix to get to compile
//extern int __cdecl I_W32ExceptionHandler(PEXCEPTION_POINTERS ep);

#include <InitGuid.h>

extern FILE* debugfile;
extern FILE* logfile;
extern FILE* openglfile;

//bool ff_shake[MAXPLAYERS];
//int ff_frequency[MAXPLAYERS];
//int ff_intensity[MAXPLAYERS];
//int ff_timeout[MAXPLAYERS];

// output string buffer
#define MSGBUFSIZE 4096
#define GLMSGBUFSIZE 8096
static char msgbuf[MSGBUFSIZE];
static char glbuf[GLMSGBUFSIZE];


// ================ INTERNALS =================


void I_GetOperatingSystem()
{
	int osver = 0.0;
	int minorver = 0.0;

	NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
	OSVERSIONINFOEXW info;

	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		info.dwOSVersionInfoSize = sizeof(info);
		RtlGetVersion(&info);
		info.dwMajorVersion; info.dwMinorVersion; info.dwBuildNumber;
		//osver = info.dwMajorVersion;
		//minorver = info.dwMinorVersion;
		I_Printf("==============================================================================\n");
		I_Printf("Operating System:\n Windows %u.%u.%u\n", info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);
		I_Printf("==============================================================================\n");
	}
	//GetVersionEx((LPOSVERSIONINFO)&info); //info requires typecasting
}


//
// FlushMessageQueue
//
// Hacktastic work around for SDL_Quit()
//
void FlushMessageQueue()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// ============ END OF INTERNALS ==============

void I_SetupSignalHandlers(bool allow_coredump)
{
	/* nothing needed */
}

void I_CheckAlreadyRunning(void)
{
	/* nothing needed */
}

void I_Warning(const char *warning, ...) 
{
	va_list argptr;

	va_start(argptr, warning);
	vsprintf(msgbuf, warning, argptr);

	I_Printf("WARNING: %s", msgbuf);
	va_end(argptr);
}
void F_Error(const char *s, ...) 
{
	va_list va;
	vprintf(s, va);
	exit(0);
}

//
// I_Error
//
void I_Error(const char* error, ...)
{
	va_list argptr;

	va_start(argptr, error);
	vsprintf(msgbuf, error, argptr);
	va_end(argptr);

	if (logfile)
	{
		fprintf(logfile, "ERROR: %s\n", msgbuf);
		fflush(logfile);
	}

	if (debugfile)
	{
		fprintf(debugfile, "ERROR: %s\n", msgbuf);
		fflush(debugfile);
	}
	if (openglfile)
	{
		fprintf(openglfile, "ERROR: %s\n", glbuf);
		fflush(openglfile);
	}

	I_SystemShutdown();

	I_MessageBox(msgbuf, "EDGE Error");

	I_CloseProgram(-1);
}


//
// I_Printf
//
void I_Printf(const char* message, ...)
{
	va_list argptr;
	char* string;
	char printbuf[MSGBUFSIZE];

	// clear the buffer
	memset(printbuf, 0, MSGBUFSIZE);

	string = printbuf;

	va_start(argptr, message);

	// Print the message into a text string
	vsprintf(printbuf, message, argptr);

	L_WriteLog("%s", printbuf);

	// If debuging enabled, print to the debugfile
	L_WriteDebug("%s", printbuf);

	// Clean up \n\r combinations
	while (*string)
	{
		if (*string == '\n')
		{
			memmove(string + 2, string + 1, strlen(string));
			string[1] = '\r';
			string++;
		}
		string++;
	}

	// Send the message to the console.
	CON_Printf("%s", printbuf);

	va_end(argptr);
}


void I_MessageBox(const char* message, const char* title)
{
	MessageBox(NULL, message, title,
		MB_ICONEXCLAMATION | MB_OK |
		MB_SYSTEMMODAL | MB_SETFOREGROUND);
}


//
// I_Sleep
//
void I_Sleep(int millisecs)
{
	::Sleep(millisecs);
}

//
// I_SystemShutdown
//
// -ACB- 1998/07/11 Tidying the code
//
void I_SystemShutdown(void)
{
	// make sure audio is unlocked (e.g. I_Error occurred)
	//I_UnlockAudio();

	//I_ShutdownNetwork();
	//I_ShutdownMusic();
	//I_ShutdownSound();
	//I_ShutdownControl();
	//I_ShutdownGraphics();

	if (logfile)
	{
		fclose(logfile);
		logfile = NULL;
	}

	// -KM- 1999/01/31 Close the debugfile
	if (debugfile != NULL)
	{
		fclose(debugfile);
		debugfile = NULL;
	}

	//ShowCursor(TRUE);
	FlushMessageQueue();
}