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

#define USE_GL
#ifdef USE_GL

/***************************************************************************
 mainSDL.cpp  -  description
 -------------------
 begin                : Mon Jun 3 2002
 copyright            : (C) 2002 by Yaz0r
 email                : yaz0r@yaz0r.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "i_sdlinc.h"
#include <SDL2/SDL_mixer.h>
#include "osystem.h"

#include "fitd.h"
#include "common.h"

namespace Fitd 
{
unsigned int ditherTexture;
unsigned int gouraudTexture = 0;

GFXSystem *g_driver;

struct quadStruct 
{
	float x1;
	float y1;
	float z1;

	float x2;
	float y2;
	float z2;

	float x3;
	float y3;
	float z3;

	float x4;
	float y4;
	float z4;

	int color;
	int transparency;

	int numPoint;

	float depth;
	bool sorted;
};

quadStruct quadTable[5000];
int positionInQuadTable = 0;

char* tempBuffer;
SDL_Surface* sdl_buffer;
SDL_Surface* sdl_buffer320x200;
SDL_Surface* sdl_buffer640x400;
SDL_Surface* sdl_bufferStretch;
SDL_Surface* sdl_bufferRGBA;
static SDL_Window* sdl_screen;// that's the SDL global object for the screen
static SDL_GLContext sdlGlContext;
SDL_Color sdl_colors[256];
SDL_Surface* surfaceTable[16];
char RGBA_Pal[256 * 4];
//TTF_Font *font;

GLuint    backTexture;
GLuint    modelsDisplayList;

#ifdef INTERNAL_DEBUGGER
GLuint    debugFontTexture;
#endif

GLUtesselator *tobj;

GLdouble tesselateList[100][6];

GLUquadricObj *sphere;

void GFXSystem::delay(int time) {
	SDL_Delay(time);
}

void GFXSystem::updateImage() {
}

/*void OSystem::getMouseStatus(mouseStatusStruct * mouseData)
 {

 SDL_GetMouseState(&mouseData->X, &mouseData->Y);

 mouseData->left = mouseLeft;
 mouseData->right = mouseRight;

 mouseLeft = 0;
 mouseRight = 0;
 }*/

#ifdef WIN32
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif

void CALLBACK combineCallback(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut) 
{
	int i;
	GLdouble *vertex;

	SYS_ASSERT(true);

	vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	for(i = 3; i < 6; i++) {
		vertex[i] = weight[0] * vertex_data[0][i] +
		            weight[1] * vertex_data[1][i] +
		            weight[2] * vertex_data[2][i] +
		            weight[3] * vertex_data[3][i];
	}

	*dataOut = vertex;
}

void OPL_musicPlayer(void *udata, Uint8 *stream, int len)
{
	musicUpdate(udata, stream, len);
}

void CALLBACK vertexCallback(void *vertex) 
{
	GLdouble *ptr;
	GLdouble x;
	GLdouble y;

	ptr = (GLdouble *) vertex;
	//  glVertex3dv((GLdouble *) ptr);
	//  glColor3dv((GLdouble *) ptr + 3);

	x = ptr[0];
	y = ptr[1];

	glColor4ub(255, 255, 255, 255);
	glTexCoord2d(x / (float)1024, y / (float)512);
	glVertex3d(x, y, 100);
}

void Sound_Quit(void) 
{
	Mix_HookMusic(NULL, NULL);
	Mix_CloseAudio();
}

GFXSystem::GFXSystem() 
{
	_palette = new char[0x300];
	_paletteObj = new Palette(_palette);
}

void GFXSystem::init()  // that's the constructor of the system dependent
// object used for the SDL port
{
	unsigned char *keyboard;
	int size;

	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) 
	{
		I_Error("Couldn't initialize SDL: %s\n", SDL_GetError());
	}

	SDL_PumpEvents();

	sdl_screen = SDL_CreateWindow(
		"EDGE In The Dark \"GL\"",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_OPENGL /*|SDL_FULLSCREEN*/);

	if (sdl_screen == NULL) 
	{
		I_Error("Couldn't set 640x480x32 video mode: %s\n", SDL_GetError());
	}

	/* now create the GL context */
	sdlGlContext = SDL_GL_CreateContext(sdl_screen);

	if (sdlGlContext == NULL) 
	{
		I_Error("Failed to create GL context : % s\n", SDL_GetError());
	}

	_mouseLeft = 0;
	_mouseRight = 0;

	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glViewport(0, 0, 640, 400); //0,0,640,400... up to 800x600 after compile 

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);       // Black Background
	glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
	glLoadIdentity();                 // Reset The Projection Matrix
	glOrtho(0, 320, 200, 0, 0, -100 * 1000);
	glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
	glLoadIdentity();                 // Reset The Modelview Matrix

	modelsDisplayList = glGenLists(1);

	// Create a new tessellation object
	tobj = gluNewTess();

	// Set callback functions
	gluTessCallback(tobj, GLU_TESS_VERTEX, (void(CALLBACK *)())vertexCallback);
	gluTessCallback(tobj, GLU_TESS_BEGIN, (void(CALLBACK *)())glBegin);
	gluTessCallback(tobj, GLU_TESS_END, glEnd);
	gluTessCallback(tobj, GLU_TESS_COMBINE, (void(CALLBACK *)())combineCallback);

	gluTessCallback(tobj, GLU_TESS_VERTEX, (void(CALLBACK *)())vertexCallback);
	gluTessCallback(tobj, GLU_TESS_BEGIN, (void(CALLBACK *)())glBegin);
	gluTessCallback(tobj, GLU_TESS_END, glEnd);
	gluTessCallback(tobj, GLU_TESS_COMBINE, (void(CALLBACK *)())combineCallback);

	// init debug font
#if 0
#ifdef INTERNAL_DEBUGGER
	glGenTextures(1, &debugFontTexture);
	glBindTexture(GL_TEXTURE_2D, debugFontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, debugFont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
#endif

	// SDL_mixer init
	// note, freq 44100, chunksize 2048
	if(Mix_OpenAudio(8000, MIX_DEFAULT_FORMAT, 1, 512) == -1) 
	{
		I_Error("Mix_OpenAudio: %s\n", Mix_GetError());
	}

	Mix_HookMusic(OPL_musicPlayer, NULL);

	// configure offset mode
	glPolygonOffset(1, 2);

	// generate textures
	{
		int i;
		int j;

		unsigned char ditherMap[256*256*4];

		unsigned char *tempPtr = ditherMap;

		for(i = 0; i < 256; i++) 
		{
			for(j = 0; j < 256; j++) 
			{
				unsigned char ditherValue = g_fitd->randRange(0, 0x50);

				*(tempPtr++) = ditherValue;
				*(tempPtr++) = ditherValue;
				*(tempPtr++) = ditherValue;
				*(tempPtr++) = 255;
			}
		}

		//glBlendFunc(GL_SRC_ALPHA,GL_ONE);

		glGenTextures(1, &ditherTexture);
		glBindTexture(GL_TEXTURE_2D, ditherTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, ditherMap);

		glBindTexture(GL_TEXTURE_2D, 0);


		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_1D);
	}

	// quadrics init

	sphere = gluNewQuadric();
}

void GFXSystem::setPalette(byte *palette) 
{

	int i;
	unsigned char localPalette[256*256*4];
	unsigned char *ptr = localPalette;

	memcpy(RGBA_Pal, palette, 256 * 4);

	for(i = 0; i < 256; i++) 
	{
		memcpy(ptr, palette, 256 * 4);

		ptr += 256 * 4;
	}

	if(gouraudTexture) 
	{
		glDeleteTextures(1, &gouraudTexture);
	}
	glGenTextures(1, &gouraudTexture);
	glBindTexture(GL_TEXTURE_2D, gouraudTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, localPalette);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GFXSystem::getPalette(char *palette) 
{
	memcpy(palette, RGBA_Pal, 256 * 4);
}

char tempBuffer2[1024*512*3];

static unsigned long int zoom = 0;

float nearVal = 100;
float farVal = 100000;
float cameraZoom = 0;
float fov = 0;

#define _USE_MATH_DEFINES
#include <math.h>

void GFXSystem::flip(unsigned char *videoBuffer) 
{
	int i;
	int j;
	int bestIdx;

	for(j = 0; j < positionInQuadTable; j++) 
	{
		float bestDepth = -10000;
		//    int color;
		bestIdx = 0;

		for(i = 0; i < positionInQuadTable; i++) 
		{
			if(!quadTable[i].sorted) {

				if(bestDepth < quadTable[i].depth) 
				{
					bestDepth = quadTable[i].depth;
					bestIdx = i;
				}
			}
		}

		glColor4ub(g_driver->_palette[quadTable[bestIdx].color * 3],
				   g_driver->_palette[quadTable[bestIdx].color * 3 + 1],
				   g_driver->_palette[quadTable[bestIdx].color * 3 + 2],
				   quadTable[bestIdx].transparency);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_QUADS);
		glVertex3f(quadTable[bestIdx].x1, quadTable[bestIdx].y1, quadTable[bestIdx].z1);
		glVertex3f(quadTable[bestIdx].x2, quadTable[bestIdx].y2, quadTable[bestIdx].z2);
		glVertex3f(quadTable[bestIdx].x3, quadTable[bestIdx].y3, quadTable[bestIdx].z3);

		if(quadTable[bestIdx].numPoint == 4)
			glVertex3f(quadTable[bestIdx].x4, quadTable[bestIdx].y4, quadTable[bestIdx].z4);
		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);



		quadTable[bestIdx].sorted = true;
	}

	positionInQuadTable = 0;

	SDL_GL_SwapWindow(sdl_screen);

}

void GFXSystem::startFrame() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef INTERNAL_DEBUGGER
	if(backgroundMode == backgroundModeEnum_2D)
#endif
	{
		glDisable(GL_DEPTH_TEST);
		glColor4ub(255, 255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, backTexture);
		glBegin(GL_TRIANGLES);

		glTexCoord2f(0, 0); // triangle haut gauche
		glVertex3f(0, 0, 49);
		glTexCoord2f(640.f / 1024.f, 0);
		glVertex3f(640, 0, 49);
		glTexCoord2f(0.0f, 400.f / 512.f);
		glVertex3f(0, 400, 49);

		glTexCoord2f(640.f / 1024.f, 0); // triangle haut gauche
		glVertex3f(640, 0, 49);
		glTexCoord2f(640.f / 1024.f, 400.f / 512.f);
		glVertex3f(640, 400, 49);
		glTexCoord2f(0.0f, 400.f / 512.f);
		glVertex3f(0, 400, 49);

		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_DEPTH_TEST);
	}
}


char tempBuffer3[320*200*3];

void GFXSystem::CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom) 
{
	char *out = tempBuffer3;
	char *in = (char *)videoBuffer + left + top * 320;

	int i;
	int j;

	while((right - left) % 4) 
	{
		right++;
	}

	while((bottom - top) % 4) 
	{
		bottom++;
	}

	for(i = top; i < bottom; i++) 
	{
		in = (char *)videoBuffer + left + i * 320;
		for(j = left; j < right; j++) 
		{
			unsigned char color = *(in++);

			*(out++) = g_driver->_palette[color*3];
			*(out++) = g_driver->_palette[color*3+1];
			*(out++) = g_driver->_palette[color*3+2];
		}
	}


	glBindTexture(GL_TEXTURE_2D, backTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, right - left, bottom - top, GL_RGB, GL_UNSIGNED_BYTE, tempBuffer3);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GFXSystem::initBuffer(char *buffer, int width, int height) 
{
	memset(tempBuffer2, 0, 1024 * 512 * 3);

	glGenTextures(1, &backTexture);
	glBindTexture(GL_TEXTURE_2D, backTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBuffer2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GFXSystem::crossFade(char *buffer, char *palette) 
{
}

int posInStream = 0;
volatile bool deviceStatus = false;

void my_audio_callback(void *userdata, Uint8 *stream, int len) 
{
	/*  Sound_Sample *sample = (Sound_Sample *)userdata;
	 Uint8* input = (Uint8*)sample->buffer;

	 if((unsigned int)posInStream+len < sample->buffer_size)
	 {
	 memcpy(stream,input+posInStream,len);
	 posInStream+=len;
	 }
	 else
	 {
	 len = sample->buffer_size - posInStream;
	 memcpy(stream,input+posInStream,len);
	 posInStream+=len;
	 }*/
}

#ifdef USE_UNPACKED_DATA
void GFXSystem::playSampleFromName(char *sampleName) {

	Mix_Chunk *sample;

	sample = Mix_LoadWAV_RW(SDL_RWFromFile(sampleName, "rb"), 1);

	if(!sample) {
#ifdef INTERNAL_DEBUGGER
		printf("Mix_LoadWAV_RW: %s\n", Mix_GetError());
#endif
	} else {
		Mix_PlayChannel(-1, sample, 0);
	}
}
#else
void GFXSystem::playSample(char *samplePtr, int size) 
{

	Mix_Chunk *sample;

	sample = Mix_LoadWAV_RW(SDL_RWFromConstMem(samplePtr, size), 1);

	if(!sample) 
	{
#ifdef INTERNAL_DEBUGGER
		I_Warning("Mix_LoadWAV_RW: %s\n", Mix_GetError());
#endif
	} 
	else 
	{
		Mix_PlayChannel(-1, sample, 0);
	}
}
#endif

int tesselatePosition = 0;

void GFXSystem::startBgPoly() 
{
	// glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, backTexture);
	//glBegin(GL_POLYGON);

	gluTessBeginPolygon(tobj, NULL);
	gluTessBeginContour(tobj);

	tesselatePosition = 0;
}

void GFXSystem::endBgPoly() 
{
	gluTessEndContour(tobj);
	gluTessEndPolygon(tobj);

	// glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void GFXSystem::addBgPolyPoint(int x, int y) 
{
	tesselateList[tesselatePosition][0] = x;
	tesselateList[tesselatePosition][1] = y;
	tesselateList[tesselatePosition][2] = 0;
	tesselateList[tesselatePosition][3] = 1.f;
	tesselateList[tesselatePosition][4] = 1.f;
	tesselateList[tesselatePosition][5] = 1.f;

	gluTessVertex(tobj, tesselateList[tesselatePosition], tesselateList[tesselatePosition]);

	tesselatePosition++;
}


void GFXSystem::stopFrame() 
{
}

void GFXSystem::startModelRender() 
{
	//glNewList(modelsDisplayList, GL_COMPILE);
}

void GFXSystem::stopModelRender() 
{
	//glEndList();
}

void GFXSystem::fillPoly(float *buffer, int numPoint, unsigned char color, uint8 polyType) 
{
	int i;

	switch(polyType) 
	{
	case 0: { // flat:
		glColor4ub(g_driver->_palette[color*3],
				   g_driver->_palette[color*3+1],
				   g_driver->_palette[color*3+2],
				   255);
		glBegin(GL_POLYGON);

		for(i = 0; i < numPoint; i++) 
		{
			glVertex3f(buffer[0], buffer[1], buffer[2]);
			buffer += 3;
		}

		glEnd();
		break;
	}
	case 1: 
	{ // dither (pierre)
		float *readList;
		GLdouble textureX = 0;
		GLdouble textureY = 0;
		GLdouble textureZ = 0;

		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		GLint viewMatrix[4];

		glBindTexture(GL_TEXTURE_2D, ditherTexture);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glColor4ub(g_driver->_palette[color*3],
				   g_driver->_palette[color*3+1],
				   g_driver->_palette[color*3+2],
				   255);
		readList = (float *)buffer;

		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		glGetIntegerv(GL_VIEWPORT, viewMatrix);

		glBegin(GL_POLYGON);

		for(i = 0; i < numPoint; i++) 
		{
			float X = *(readList++);
			float Y = *(readList++);
			float Z = *(readList++);

			//gluProject  ( X , Y, -Z , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );

			glTexCoord2f(X * 200, Y * 200);
			glVertex3f(X, Y, Z);
		}

		glEnd();

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, 0);
		break;
	}
	case 2: 
	{ // trans
		glColor4ub(g_driver->_palette[color*3],
				   g_driver->_palette[color*3+1],
				   g_driver->_palette[color*3+2],
				   128);
		glBegin(GL_POLYGON);

		for(i = 0; i < numPoint; i++) 
		{
			glVertex3f(buffer[0], buffer[1], buffer[2]);
			buffer += 3;
		}

		glEnd();
		break;
		/*     float z1;
		 float z2;
		 float z3;
		 float z4;

		 GLdouble textureX = 0;
		 GLdouble textureY = 0;
		 GLdouble textureZ = 0;

		 GLdouble modelMatrix[16];
		 GLdouble projMatrix[16];
		 GLint viewMatrix[4];

		 glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		 glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		 glGetIntegerv(GL_VIEWPORT, viewMatrix);

		 SYS_ASSERT(numPoint == 3 || numPoint == 4);

		 quadTable[positionInQuadTable].x1 = buffer[0];
		 quadTable[positionInQuadTable].y1 = buffer[1];
		 quadTable[positionInQuadTable].z1 = buffer[2];
		 gluProject  ( buffer[0] , buffer[1] , buffer[2] , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );
		 z1 = textureZ;

		 quadTable[positionInQuadTable].x2 = buffer[3];
		 quadTable[positionInQuadTable].y2 = buffer[4];
		 quadTable[positionInQuadTable].z2 = buffer[5];
		 gluProject  ( buffer[3] , buffer[4] , buffer[5] , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );
		 z2 = textureZ;

		 quadTable[positionInQuadTable].x3 = buffer[6];
		 quadTable[positionInQuadTable].y3 = buffer[7];
		 quadTable[positionInQuadTable].z3 = buffer[8];
		 gluProject  ( buffer[6] , buffer[7] , buffer[8] , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );
		 z3 = textureZ;

		 if(numPoint == 4)
		 {
		 quadTable[positionInQuadTable].x4 = buffer[9];
		 quadTable[positionInQuadTable].y4 = buffer[10];
		 quadTable[positionInQuadTable].z4 = buffer[11];
		 gluProject  ( buffer[9] , buffer[10] , buffer[11] , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );
		 z4 = textureZ;
		 }

		 quadTable[positionInQuadTable].color = color;
		 quadTable[positionInQuadTable].transparency = 128;

		 quadTable[positionInQuadTable].numPoint = numPoint;

		 quadTable[positionInQuadTable].sorted = false;

		 quadTable[positionInQuadTable].depth = z1 + z2 + z3;

		 if(numPoint == 4)
		 quadTable[positionInQuadTable].depth += z4;

		 quadTable[positionInQuadTable].depth/=numPoint;

		 /*      if(buffer[5] > quadTable[positionInQuadTable].depth)
		 quadTable[positionInQuadTable].depth = (int)buffer[5];

		 if(buffer[8] > quadTable[positionInQuadTable].depth)
		 quadTable[positionInQuadTable].depth = (int)buffer[8];

		 if(numPoint == 4)
		 if(buffer[11] > quadTable[positionInQuadTable].depth)
		 quadTable[positionInQuadTable].depth = (int)buffer[11]; */

		//positionInQuadTable++;
		break;
	}
	default:
	case 4: 
	{ // marbre
		int numColorToGo;
		float *readList = (float *)buffer;

		int maxY = 0;
		int minY = 10000;

		GLdouble textureX = 0;
		GLdouble textureY = 0;
		GLdouble textureZ = 0;

		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		GLint viewMatrix[4];

		float colorStep;
		float colorf;

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		glGetIntegerv(GL_VIEWPORT, viewMatrix);

		for(i = 0; i < numPoint; i++) 
		{
			float X = *(readList++);
			float Y = *(readList++);
			float Z = *(readList++);

			textureX = X;
			textureY = Y;
			textureZ = Z;
			/// gluProject  ( X , Y , Z , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );

			if(textureY > maxY)
				maxY = (int)textureY;

			if(textureY < minY)
				minY = (int)textureY;
		}

		numColorToGo = 0xF - (color & 0xF);

		colorStep = numColorToGo / (float)abs(maxY - minY);

		colorf = 0;

		readList = (float *)buffer;

		glColor4ub(255, 255, 255, 255);

		glBindTexture(GL_TEXTURE_2D, gouraudTexture);

		glBegin(GL_POLYGON);

		for(i = 0; i < numPoint; i++) {
			float X = *(readList++);
			float Y = *(readList++);
			float Z = *(readList++);

			//gluProject  ( X , Y , Z , modelMatrix , projMatrix , viewMatrix , &textureX , &textureY , &textureZ );

			textureX = X;
			textureY = Y;
			textureZ = Z;

			colorf = (float)(color + (colorStep * (textureY - minY)));

			if(colorf > color + numColorToGo)
				colorf = (float)(color + numColorToGo);

			glTexCoord1f(colorf / 256);
			//              glColor3ub(palette[colorf*3],palette[colorf*3+1],palette[colorf*3+2]);

			glVertex3f(X, Y, Z);
		}

		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);

		break;
	}
	}
}

void GFXSystem::draw3dLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color) 
{
	glColor4ub(g_driver->_palette[color*3],
			   g_driver->_palette[color*3+1],
			   g_driver->_palette[color*3+2], 255);

	glBegin(GL_LINES);

	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);

	glEnd();
}

void GFXSystem::cleanScreenKeepZBuffer() 
{
	return;
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glColor4ub(255, 255, 255, 255);
	glBindTexture(GL_TEXTURE_2D, backTexture);
	glBegin(GL_TRIANGLES);

	glTexCoord2f(0, 0); // triangle haut gauche
	glVertex3f(0, 0, 49);
	glTexCoord2f(640.f / 1024.f, 0);
	glVertex3f(640, 0, 49);
	glTexCoord2f(0.0f, 400.f / 512.f);
	glVertex3f(0, 400, 49);

	glTexCoord2f(640.f / 1024.f, 0); // triangle haut gauche
	glVertex3f(640, 0, 49);
	glTexCoord2f(640.f / 1024.f, 400.f / 512.f);
	glVertex3f(640, 400, 49);
	glTexCoord2f(0.0f, 400.f / 512.f);
	glVertex3f(0, 400, 49);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
}

void GFXSystem::draw3dQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, unsigned char color, int transparency) 
{
	glColor4ub(g_driver->_palette[(color+3)*3],
			   g_driver->_palette[(color+3)*3+1],
			   g_driver->_palette[(color+3)*3+2], 255);

	glBegin(GL_LINE_LOOP);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x3, y3, z3);
	glVertex3f(x4, y4, z4);
	glEnd();

	if(transparency != 255) 
	{
		quadTable[positionInQuadTable].x1 = x1;
		quadTable[positionInQuadTable].y1 = y1;
		quadTable[positionInQuadTable].z1 = z1;

		quadTable[positionInQuadTable].x2 = x2;
		quadTable[positionInQuadTable].y2 = y2;
		quadTable[positionInQuadTable].z2 = z2;

		quadTable[positionInQuadTable].x3 = x3;
		quadTable[positionInQuadTable].y3 = y3;
		quadTable[positionInQuadTable].z3 = z3;

		quadTable[positionInQuadTable].x4 = x4;
		quadTable[positionInQuadTable].y4 = y4;
		quadTable[positionInQuadTable].z4 = z4;

		quadTable[positionInQuadTable].color = color;
		quadTable[positionInQuadTable].transparency = transparency;

		quadTable[positionInQuadTable].numPoint = 4;

		quadTable[positionInQuadTable].sorted = false;

		quadTable[positionInQuadTable].depth = z1;

		if(z2 < quadTable[positionInQuadTable].depth)
			quadTable[positionInQuadTable].depth = z2;

		if(z3 < quadTable[positionInQuadTable].depth)
			quadTable[positionInQuadTable].depth = z3;

		if(z4 < quadTable[positionInQuadTable].depth)
			quadTable[positionInQuadTable].depth = z4;

		positionInQuadTable++;
	} else 
	{
		glColor4ub(g_driver->_palette[color*3],
				   g_driver->_palette[color*3+1],
				   g_driver->_palette[color*3+2],
				   255);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_QUADS);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
		glVertex3f(x3, y3, z3);
		glVertex3f(x4, y4, z4);
		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void GFXSystem::drawSphere(float X, float Y, float Z, uint8 color, float size) 
{
	glMatrixMode(GL_MODELVIEW);

	glColor3ub(g_driver->_palette[color*3],
			   g_driver->_palette[color*3+1],
			   g_driver->_palette[color*3+2]);
	glPushMatrix();

	glTranslatef(X, Y, Z);

	gluDisk(sphere, 0, (float)size, 10, 10);

	glPopMatrix();
}

#ifdef INTERNAL_DEBUGGER
void GFXSystem::drawDebugText(const uint32 X, const uint32 Y, const uint8 *string)
{
#if 0
	u32 currentX = X;
	u32 i;
	u32 stringLength;

	glBindTexture(GL_TEXTURE_2D, debugFontTexture);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glColor4ub(255, 255, 255, 255);

	stringLength = strlen(string);

	for(i = 0; i < stringLength; i++) {
		float textX1;
		float textY1;
		float textX2;
		float textY2;

		uint8 lineNumber;
		uint8 colNumber;

		lineNumber = string[i] >> 4;
		colNumber = string[i] & 0xF;

		textX1 = ((256.f / 16.f) * colNumber) / 256.f;
		textY1 = ((256.f / 16.f) * lineNumber) / 256.f;
		textX2 = ((256.f / 16.f) * (colNumber + 1)) / 256.f;
		textY2 = ((256.f / 16.f) * (lineNumber + 1)) / 256.f;

		glBegin(GL_QUADS);

		glTexCoord2f(textX1, textY1);
		glVertex2d(currentX, Y);

		glTexCoord2f(textX2, textY1);
		glVertex2d(currentX + 8, Y);

		glTexCoord2f(textX2, textY2);
		glVertex2d(currentX + 8, Y + 8);

		glTexCoord2f(textX1, textY2);
		glVertex2d(currentX, Y + 8);

		glEnd();

		currentX += 4;
	}

	glEnable(GL_DEPTH_TEST);
#endif
}
#endif
} // end of namespace Fitd
#endif
