/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/IoDevice/DirAsDisk.c,v $
**
** $Revision: 1.15 $
**
** $Date: 2008-06-24 20:10:38 $
**
** More info: http://www.bluemsx.com
**
** Copyright (C) 2003-2007 Daniel Vik, Tomas Karlsson
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
******************************************************************************
*/
#define USE_ARCH_GLOB

#include "DirAsDisk.h"

#if defined(WIN32) || defined (WINDOWS_HOST)
#include <io.h> // not on Linux
#endif

#ifdef _WIN32
#pragma warning(disable: 4996)
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#ifdef USE_ARCH_GLOB
#include "ArchGlob.h"
#else
#include <windows.h>
#endif

#if defined(WIN32) || defined (WINDOWS_HOST)
#include <io.h>
#else
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

static const unsigned char msxboot[] = { 
	0xeb,0xfe,0x90,0x44,0x53,0x4b,0x54,0x4f,
	0x4f,0x4c,0x20,0x00,0x02,0x02,0x01,0x00,
	0x02,0x70,0x00,0xa0,0x05,0xf9,0x03,0x00,
	0x09,0x00,0x02,0x00,0x00,0x00,0xd0,0xed,
	0x53,0x59,0xc0,0x32,0xc4,0xc0,0x36,0x56,
	0x23,0x36,0xc0,0x31,0x1f,0xf5,0x11,0x79,
	0xc0,0x0e,0x0f,0xcd,0x7d,0xf3,0x3c,0xca,
	0x63,0xc0,0x11,0x00,0x01,0x0e,0x1a,0xcd,
	0x7d,0xf3,0x21,0x01,0x00,0x22,0x87,0xc0,
	0x21,0x00,0x3f,0x11,0x79,0xc0,0x0e,0x27,
	0xcd,0x7d,0xf3,0xc3,0x00,0x01,0x58,0xc0,
	0xcd,0x00,0x00,0x79,0xe6,0xfe,0xfe,0x02,
	0xc2,0x6a,0xc0,0x3a,0xc4,0xc0,0xa7,0xca,
	0x22,0x40,0x11,0x9e,0xc0,0x0e,0x09,0xcd,
	0x7d,0xf3,0x0e,0x07,0xcd,0x7d,0xf3,0x18,
	0xb2,0x00,0x4d,0x53,0x58,0x44,0x4f,0x53,
	0x20,0x20,0x53,0x59,0x53,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x6f,
	0x6f,0x74,0x20,0x65,0x72,0x72,0x6f,0x72,
	0x0d,0x0a,0x50,0x72,0x65,0x73,0x73,0x20,
	0x61,0x6e,0x79,0x20,0x6b,0x65,0x79,0x20,
	0x66,0x6f,0x72,0x20,0x72,0x65,0x74,0x72,
	0x79,0x0d,0x0a,0x24,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static unsigned char svi738CpmBoot[512] = {
    0xEB,0xFE,0x90,0x41,0x53,0x43,0x20,0x20, 
    0x32,0x2E,0x32,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0xD0,0x21, 
    0x2D,0xC0,0x11,0x00,0x80,0x01,0x24,0x00, 
    0xED,0xB0,0xC3,0x00,0x80,0xF3,0x11,0x00, 
    0x90,0x0E,0x1A,0xCD,0x7D,0xF3,0x11,0x01, 
    0x00,0x26,0x10,0x2E,0x00,0x0E,0x2F,0xCD, 
    0x7D,0xF3,0xF3,0x21,0x00,0x90,0x11,0x00, 
    0xE2,0x01,0xD0,0x1D,0xED,0xB0,0xC3,0x00, 
    0xE2,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 
    0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 
    0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 
    0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 
    0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 
    0xFF,0x00,0xFF,0x00,0xFF,0xFF,0xFF,0x00, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 
    0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0xFF, 
    0xE1,0xC3,0x29,0xE1,0x3A,0x42,0xD7,0xC3, 
    0x01,0xD7,0xEB,0x22,0xB1,0xE1,0xC3,0xDA, 
    0xD9,0x2A,0xBF,0xE1,0xC3,0x29,0xE1,0x2A, 
    0xAD,0xE1,0xC3,0x29,0xE1,0xCD,0x51,0xE0, 
    0xCD,0x3B,0xDC,0xC3,0x01,0xDB,0x2A,0xBB, 
    0xE1,0x22,0x45,0xD7,0xC9,0x3A,0xD6,0xE1, 
    0xFE,0xFF,0xC2,0x3B,0xE1,0x3A,0x41,0xD7, 
    0xC3,0x01,0xD7,0xE6,0x1F,0x32,0x41,0xD7, 
    0xC9,0xCD,0x51,0xE0,0xC3,0x93,0xDF,0xCD, 
    0x51,0xE0,0xC3,0x9C,0xDF,0xCD,0x51,0xE0, 
    0xC3,0xD2,0xDF,0x2A,0x43,0xD7,0x7D,0x2F, 
    0x5F,0x7C,0x2F,0x2A,0xAF,0xE1,0xA4,0x57, 
    0x7D,0xA3,0x5F,0x2A,0xAD,0xE1,0xEB,0x22, 
    0xAF,0xE1,0x7D,0xA3,0x6F,0x7C,0xA2,0x67, 
    0x22,0xAD,0xE1,0xC9,0x3A,0xDE,0xE1,0xB7, 
    0xCA,0x91,0xE1,0x2A,0x43,0xD7,0x36,0x00, 
    0x3A,0xE0,0xE1,0xB7,0xCA,0x91,0xE1,0x77, 
    0x3A,0xDF,0xE1,0x32,0xD6,0xE1,0xCD,0x45, 
    0xE0,0x2A,0x0F,0xD7,0xF9,0x2A,0x45,0xD7, 
    0x7D,0x44,0xC9,0xCD,0x51,0xE0,0x3E,0x02, 
    0x32,0xD5,0xE1,0x0E,0x00,0xCD,0x07,0xDF, 
    0xCC,0x03,0xDE,0xC9,0xE5,0x00,0x00,0x00, 
    0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
};
static unsigned char msx2cpm3boot[512] = {
    0xE9,0x00,0x00,0x52,0x56,0x53,0x20,0x64, 
    0x61,0x74,0x61,0x00,0x02,0x02,0x01,0x00, 
    0x02,0x70,0x00,0xD0,0x02,0xF8,0x02,0x00, 
    0x09,0x00,0x01,0x00,0x00,0x00,0xD0,0xF3, 
    0x31,0x1F,0xF5,0x21,0x00,0xC0,0x11,0x00, 
    0x80,0x01,0xFF,0x3F,0xED,0xB0,0xC3,0x31, 
    0x80,0x21,0x00,0xC1,0x06,0x00,0xDB,0xA8, 
    0xE6,0x3F,0x57,0xD3,0xA8,0x3A,0xFF,0xFF, 
    0x2F,0x4F,0xE6,0x3F,0x5F,0x32,0xFF,0xFF, 
    0x97,0x3D,0xD3,0xFF,0x77,0x20,0xFA,0xD3, 
    0xFF,0xBE,0x20,0x04,0x3C,0x20,0xF8,0x3D, 
    0xB8,0xDA,0x61,0x80,0xED,0x53,0x70,0x80, 
    0x47,0x7B,0xC6,0x40,0x30,0xDE,0x79,0x32, 
    0xFF,0xFF,0x7A,0xC6,0x40,0x30,0xCB,0x11, 
    0x00,0x00,0x7A,0xD3,0xA8,0x7B,0x32,0xFF, 
    0xFF,0x78,0x32,0xFF,0x00,0xD9,0x3E,0x03, 
    0xD3,0xFF,0x21,0x00,0x00,0x11,0x00,0xC0, 
    0x01,0x00,0x01,0xED,0xB0,0x3E,0x00,0xD3, 
    0xFF,0x21,0x00,0x80,0x11,0x00,0xC0,0x01, 
    0xFF,0x3F,0xED,0xB0,0xD9,0xC3,0xA0,0xC0, 
    0x7A,0xCD,0xE5,0xC0,0xD3,0xA8,0x7B,0xCD, 
    0xE5,0xC0,0x32,0xFF,0xFF,0x57,0xE6,0x03, 
    0x4F,0x06,0x00,0x21,0xC5,0xFC,0x09,0x72, 
    0x3E,0x50,0x32,0xAE,0xF3,0xFD,0x2A,0xC0, 
    0xFC,0xDD,0x21,0x6C,0x00,0xCD,0x1C,0x00, 
    0x1E,0x01,0x0E,0x1B,0xCD,0x7D,0xF3,0x11, 
    0x00,0x02,0x0E,0x1A,0xCD,0x7D,0xF3,0x11, 
    0x03,0x00,0x21,0x00,0x06,0x0E,0x2F,0xCD, 
    0x7D,0xF3,0xC3,0x00,0x02,0xE6,0xC0,0x47, 
    0x0F,0x0F,0xB0,0x47,0x0F,0x0F,0x0F,0x0F, 
    0xB0,0xC9,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
};

typedef unsigned char byte;
typedef unsigned short word;
typedef struct {
  char name[9];
  char ext[4];
  int size;
  int hour,min,sec;
  int day,month,year;
  int first;
  int pos;
  int attr;
} fileinfo;

static int dskimagesize = 0;
static byte *dskimage=NULL;
static byte *fat;
static byte *direc;
static byte *cluster;
static int sectorsperfat,numberoffats,reservedsectors;
static int bytespersector,direlements,fatelements;
static int availsectors;

static int alBlockNo;

static void load_dsk_svi(int diskType)
{
    int imageSize = 0;
    int dirOff = 0;
    byte fatData = 0;

    switch (diskType) {
    case 7:         // MSX2 CP/M 3 DSDD
        imageSize = 720;
        alBlockNo = 1;
        break;
    case 6:         // MSX2 CP/M 3 SSDD
        imageSize = 360;
        alBlockNo = 1;
        break;
    case 3:         // SVI-738 CP/M SSDD
        imageSize = 360;
        alBlockNo = 1;
        break;
    case 5:         // SVI-328 Disk Basic DSDD
        dirOff = 0x2A000;
        fatData = 0xFF;
    case 2:         // SVI-328 CP/M DSDD
        imageSize = 338;
        alBlockNo = 1;
        break;
    case 4:         // SVI-328 Disk Basic SSDD 
        dirOff = 0x14C00;
        fatData = 0xFE;
    case 1:         // SVI-328 CP/M SSDD
        imageSize = 168;
        alBlockNo = 2;
        break;
    }

    dskimage = (byte *) calloc (1, imageSize * 1024);
    memset(dskimage, 0xe5, imageSize * 1024);
    dskimagesize = imageSize * 1024;

    if (diskType == 3) {
        memcpy(dskimage, svi738CpmBoot, 512);
    }
    if (diskType == 6 || diskType == 7) {
        memcpy(dskimage, msx2cpm3boot, 512);
        if (diskType == 7) {
            dskimage[dirOff + 0x13] = 0xA0;
        }
    }

    if (diskType == 4 || diskType == 5) {
        memset(dskimage + dirOff, 0xFF, 17 * 256);
        memset(dskimage + dirOff + 13 * 256, 0x00, 256);

        memset(dskimage + dirOff + 14 * 256, 0xFF, 40);
        memset(dskimage + dirOff + 14 * 256 + 40, fatData, 40);
        memset(dskimage + dirOff + 14 * 256, 0xFE, 3);
        dskimage[dirOff + 14 * 256 + 20] = 0xFE;
        memset(dskimage + dirOff + 14 * 256 + 80, 0x20, 48);
        memset(dskimage + dirOff + 14 * 256 + 128, 0x00, 128);

        memcpy(dskimage + dirOff + 15 * 256, dskimage + dirOff + 14 * 256, 256);
        memcpy(dskimage + dirOff + 16 * 256, dskimage + dirOff + 14 * 256, 256);
    }
}

static void load_dsk_msx(void) {
    dskimagesize = 720*1024;
    dskimage=(byte *) calloc (1,720*1024);
    memset (dskimage,0,720*1024);
    memcpy (dskimage,msxboot,512);
    reservedsectors=*(word *)(dskimage+0x0E);
    numberoffats=*(dskimage+0x10);
    sectorsperfat=*(word *)(dskimage+0x16);
    bytespersector=*(word *)(dskimage+0x0B);
    direlements=*(word *)(dskimage+0x11);
    fat=dskimage+bytespersector*reservedsectors;
    direc=fat+bytespersector*(sectorsperfat*numberoffats);
    cluster=direc+direlements*32;
    availsectors=80*9*2-reservedsectors-sectorsperfat*numberoffats;
    availsectors-=direlements*32/bytespersector;
    fatelements=availsectors/2;
    fat[0]=0xF9;
    fat[1]=0xFF;
    fat[2]=0xFF;
}

static fileinfo *getfileinfo(int pos) {
  fileinfo *file;  
  byte *dir;
  int i;

  dir=direc+pos*32;
  if (*dir<0x20 || *dir>=0x80) return NULL;

  file=(fileinfo *) malloc (sizeof (fileinfo));
  for (i=0; i<8; i++)
    file->name[i]=dir[i]==0x20?0:dir[i];
  file->name[8]=0;

  for (i=0; i<3; i++)
    file->ext[i]=dir[i+8]==0x20?0:dir[i+8];
  file->ext[3]=0;

  file->size=*(int *)(dir+0x1C);

  i=*(word *)(dir+0x16);
  file->sec=(i&0x1F)<<1;
  file->min=(i>>5)&0x3F;
  file->hour=i>>11;

  i=*(word *)(dir+0x18);
  file->day=i&0x1F;
  file->month=(i>>5)&0xF;
  file->year=1980+(i>>9);

  file->first=*(word *)(dir+0x1A);
  file->pos=pos;
  file->attr=*(dir+0xB);

  return file;
}

static int getfilelength(int fd) {
    int cur = lseek(fd, 0, SEEK_CUR);
    int length = lseek(fd, 0, SEEK_END);
    lseek(fd, cur, SEEK_SET);
    return length;
}

static int match(fileinfo *file, char *name) {
  char *p=file->name;
  int status=0,i;

  for (i=0; i<8; i++) {
    if (!*name)
      break;
    if (*name=='*') {
      status=1;
      name++;
      break;
    }
    if (*name=='.')
      break;
    if (toupper (*name++)!=toupper (*p++))
      return 0;
  }
  if (!status && i<8 && *p!=0) 
    return 0;
  p=file->ext;
  if (!*name && !*p) return 1;
  if (*name++!='.') return 0;
  for (i=0; i<3; i++) {
    if (*name=='*')
      return 1;
    if (toupper (*name++)!=toupper (*p++))
      return 0;
  }
  return 1;
}

static int next_link(int link) {
  int pos;

  pos=(link>>1)*3;
  if (link&1)
    return (((int)(fat[pos+2]))<<4)+(fat[pos+1]>>4);
  else
    return (((int)(fat[pos+1]&0xF))<<8)+fat[pos];
}

static int bytes_free(void) {
  int i,avail=0;

  for (i=2; i<2+fatelements; i++)
    if (!next_link (i)) avail++;
  return avail*1024;
}

static int remove_link(int link) {
  int pos;
  int current;

  pos=(link>>1)*3;
  if (link&1) {
    current=(((int)(fat[pos+2]))<<4)+(fat[pos+1]>>4);
    fat[pos+2]=0;
    fat[pos+1]&=0xF;
    return current;
  }
  else  {
    current=(((int)(fat[pos+1]&0xF))<<8)+fat[pos];
    fat[pos]=0;
    fat[pos+1]&=0xF0;
    return current;
  }
}

static void wipe(fileinfo *file) {
  int current;

  current=file->first;
  do {
    current=remove_link (current);
  } while (current!=0xFFF);
  direc[file->pos*32]=0xE5;
}

static int get_free(void) {
  int i;

  for (i=2; i<2+fatelements; i++)
    if (!next_link (i)) return i;
  //printf ("Internal error\n");
  //exit (5);
  return 0;
}

static int get_next_free(void) {
  int i,status=0;

  for (i=2; i<2+fatelements; i++)
    if (!next_link (i)) {
      if (status) 
        return i;
      else
        status=1;
    }
  //printf ("Internal error\n");
  //exit (5);
  return 0;
}

static void store_fat(int link, int next) {
  int pos;

  pos=(link>>1)*3;
  if (link&1) {
    fat[pos+2]=next>>4;
    fat[pos+1]&=0xF;
    fat[pos+1]|=(next&0xF)<<4;
  }
  else  {
    fat[pos]=next&0xFF;
    fat[pos+1]&=0xF0;
    fat[pos+1]|=next>>8;
  }
}

static int add_single_file(char *name, const char *pathname) {
  int i,total;
  fileinfo *file;
  int fileid;
  byte *buffer;
  byte *b;
  int size;
  struct stat s;
  struct tm *t;
  int first;
  int current;
  int next;
  int pos;
  char *p;
  char fullname[250];
  int result;

  strcpy (fullname,pathname);
  strcat (fullname,"/");
  strcat (fullname,name);
  fileid=open (fullname,O_BINARY|O_RDONLY);
  
  if (fileid < 0) {
      return -1;
  }

  for (i=0; i<direlements; i++) {
    if ((file=getfileinfo (i))!=NULL) {
      if (match (file,name)) {
        wipe (file);
      }
      free (file);
    }
  }

  if ((size=getfilelength(fileid))>bytes_free())
  {
    close (fileid);
    return 1;
  }

  for (i=0; i<direlements; i++)
    if (direc[i*32]<0x20 || direc[i*32]>=0x80)
      break;
  if (i==direlements)
  {
    close (fileid);
    return 2;
  }

  pos=i;

  b = buffer=(byte *) malloc ((size+1023)&(~1023));
  read (fileid,buffer,size);

  close (fileid);

  total=(size+1023)>>10;
  current=first=get_free ();

  for (i=0; i<total;) {
    memcpy (cluster+(current-2)*1024,buffer,1024);
    buffer+=1024;
    if (++i==total)
      next=0xFFF;
    else
      next=get_next_free ();
    store_fat (current,next);
    current=next;
  }

  memset (direc+pos*32,0,32);
  memset (direc+pos*32,0x20,11);
  i=0; 
  for (p=name;*p;p++) {
    if (*p=='.') {
      i=8;
      continue;
    }
    direc[pos*32+i++]=toupper (*p);
  }

  result = stat(fullname, &s);

  t = localtime(&s.st_mtime);
  if (t == NULL) {
      time_t tmp = time(NULL);
      t = localtime(&tmp);
  }
;
  if (t == NULL) {
      result = -1;
  }
  else {
    *(word *)(direc+pos*32+0x1A)=first;
    *(int *)(direc+pos*32+0x1C)=size;
    *(word *)(direc+pos*32+0x16)=
        (t->tm_sec>>1)+(t->tm_min<<5)+(t->tm_hour<<11);
    *(word *)(direc+pos*32+0x18)=
        (t->tm_mday)+(t->tm_mon<<5)+((t->tm_year-1980)<<9);
  }
  free (b);
  return result;
}

/* strupr is not a standard ANSI function, so define our own version */
static char* my_strupr(char* s)
{
    char* p = s;
    while (*p) {
        *p = toupper(*p);
        ++p;
    }
    return s;
}

static int add_single_file_svi(int diskType, char *name, const char *pathname)
{
    typedef struct
    {
        char name[6];
        char ext[3];
        byte attrib;
        byte fatpointer;
        byte reserved[5];
    } DirectoryEntry;

    FILE *fpImport;
    char fullname[250];
    char filename[80];
    char extension[10];
    char *pname;
    char *pext;
    char myname[250];
    byte fileBuf[17 * 256];
    int bytesRead;
    int fileDone;
    int dirOff;
    int dirEntryNo = 0;
    int dirFound = 0;
    DirectoryEntry myDir;
    int fatCounter = 0;
    int fatFound = 0;
    int nextTrack;
    int sides;
    int side = 0;
    int track = 0;
    int offset;

    switch (diskType) {
    case 5:
        dirOff = 0x2A000;
        sides = 2;
        break;
    case 4:
        dirOff = 0x14C00;
        sides = 1;
        break;
    }

    strcpy(fullname, pathname);
    strcat(fullname, "/");
    strcat(fullname, name);

    strcpy(myname, name);
    memset(&filename, 0x20, sizeof(filename));
    memset(&extension, 0x20, sizeof(extension));
    pname = strtok(name, ".");
    if (pname != NULL) {
        memcpy(filename, pname, strlen(pname));
        pext = strrchr(myname, '.');
        if (pext != NULL) {
            memcpy(extension, pext + 1, strlen(pext));
        }
    }
    else {
        memcpy(filename, myname, strlen(myname));
    }

    do {
        if (dskimage[dirOff + 16 * dirEntryNo] == 0xff) {
            dirFound = 1;
        }
        else {
            dirEntryNo++;
        }
    }
    while (dirEntryNo < 16 * 13 && !dirFound);

    if (!dirFound) {
        return 1;
    }

    do {
        if (dskimage[dirOff + 14 * 256 + fatCounter] == 0xff) {
            fatFound = 1;
        }
        else {
            fatCounter++;
        }
    }
    while (fatCounter < 80 && !fatFound);

    if (!fatFound) {
        return 1;
    }

    fpImport = fopen(fullname, "rb");
    if (!fpImport) {
        return 1;
    }

    memset(&myDir, 0xff, sizeof(myDir));
    memcpy(&myDir.name, filename, 6);
    memcpy(&myDir.ext, extension, 3);
    myDir.fatpointer = fatCounter;
    myDir.attrib = 0;

    pext = my_strupr(extension);
    if (0 == strncmp(extension, "BAS ", 3))
        myDir.attrib = 0x80;
    else if (0 == strncmp(extension, "BIN ", 3))
        myDir.attrib = 0x01;
    else if (0 == strncmp(extension, "DAT ", 3))
        myDir.attrib = 0x40;
    else if (0 == strncmp(extension, "SCR ", 3))
        myDir.attrib = 0xA0;
    else
        myDir.attrib = 0;

    do {
        memset(&fileBuf, 0x00, sizeof(fileBuf));

        bytesRead = fread(fileBuf, 1, sizeof(fileBuf), fpImport);
        fileDone = (bytesRead != sizeof(fileBuf));

        track = fatCounter;
        if (track > 39) {
            side = 1;
            track = 80 - fatCounter;
        }
        offset = ((track * sides + side) * 17 + 1 - 1) * 256 - 2048;
        memcpy(dskimage + offset, &fileBuf, bytesRead);

        if (!fileDone) {
            nextTrack = fatCounter;
            nextTrack++;
            if (nextTrack == 20) {
                nextTrack++;
            }
            dskimage[dirOff + 14 * 256 + fatCounter] = nextTrack;
            fatCounter = nextTrack;
        }
    }
    while (!fileDone);

    memcpy(dskimage + dirOff + dirEntryNo * 16, &myDir, sizeof(myDir));

    dskimage[dirOff + 14 * 256 + fatCounter] = 0xC0 | (int)ceil(bytesRead / 256.00);

    memcpy(dskimage + dirOff + 15 * 256, dskimage + dirOff + 14 * 256, 256);
    memcpy(dskimage + dirOff + 16 * 256, dskimage + dirOff + 14 * 256, 256);

    fclose(fpImport);
    return 0;
}

static int add_single_file_cpm(int diskType, char *name, const char *pathname)
{
    typedef struct
    {
        byte status;    // UU
        char name[8];   // Fn
        char ext[3];    // Tn
        byte extnol;    // EX
        byte lrc;       // S2
        byte extnoh;    // S1
        byte blkcnt;    // RC
        byte pointers[16];  // AL
    } DirectoryEntry;

    DirectoryEntry myDir;
    FILE *fpImport;
    char fullname[250];
    int drm = 0;
    int drmFound = 0;
    byte fileBuf[2048];
    int fileSize;
    char myname[250];
    char filename[80];
    char extension[10];
    char *pname;
    char *pext;
    int fileDone;
    int dskDataOffset;
    int alCount = 0;
    int extent = 0;
    int fileRead;
    word dirOffset;
    int dpbBLS;
    int sides;
    int trackOffset;
    switch (diskType) {
    case 7:     // MSX2 CP/M 3 DSDD
        dirOffset = 0x2400;
        dpbBLS = 2048;
        sides = 1;
        break;
    case 6:     // MSX2 CP/M 3 SSDD
        dirOffset = 0x2400;
        dpbBLS = 2048;
        sides = 0;
        break;
    case 3:     // SVI-738 CP/M SSDD
        dirOffset = 0x3600;
        dpbBLS = 2048;
        sides = 0;
        break;
    case 2:     // SVI-328 CP/M DSDD
        dirOffset = 0x5E00;
        dpbBLS = 2048;
        sides = 1;
        break;
    case 1:     // SVI-328 CP/M SSDD
    default:
        dirOffset = 0x2B00;
        dpbBLS = 1024;
        sides = 0;
        break;
    }

    strcpy(fullname, pathname);
    strcat(fullname, "/");
    strcat(fullname, name);

    name = my_strupr(name);
    strcpy(myname, name);

    memset(filename, 0x20, sizeof(filename));
    memset(extension, 0x20, sizeof(extension));

    pname = strtok(name, ".");
    if (pname != NULL) {
        memcpy(filename, pname, strlen(pname));
        pext = strrchr(myname, '.');
        if (pext != NULL) {
            memcpy(extension, pext + 1, strlen(pext));
        }
    }
    else {
        memcpy(filename, myname, strlen(myname));
    }

    fpImport = fopen(fullname, "rb");
    if (!fpImport) {
        return 1;
    }

    fseek(fpImport, 0, SEEK_END);
    fileSize = ftell(fpImport);
    if (fileSize > 0x27500) {
        fclose(fpImport);
        return 1;
    }

    do {
        if (dskimage[dirOffset + drm * 0x20] == 0xe5) {
            drmFound = 1;
        }
        else {
            drm++;
        }
    }
    while (drm < 0x40 && !drmFound);

    if (!drmFound)
    {
        fclose(fpImport);
        return 1;
    }

    memset(&myDir, 0, sizeof(myDir));
    memcpy(&myDir.name, filename, 8);
    memcpy(&myDir.ext, extension, 3);

    rewind(fpImport);
    do {
        memset(&fileBuf, 0, dpbBLS);
        myDir.pointers[alCount] = alBlockNo;

//        trackOffset = ((alBlockNo - 1) & 0x02) ? (4352 * sides) : (4352 * sides + 4352);
        trackOffset = ((alBlockNo - 1) & 0x02) ? (4352 * sides) : (4352 * sides);
        dskDataOffset = (dirOffset + (alBlockNo * dpbBLS)) + trackOffset;
//        dskDataOffset = dirOffset + (alBlockNo * dpbBLS);

        fileRead = fread(fileBuf, 1, dpbBLS, fpImport);
        fileDone = (fileRead != dpbBLS);

        memcpy(&dskimage[dskDataOffset], &fileBuf, dpbBLS);

        alBlockNo++;
        alCount++;
        if (alCount > 15) {
            myDir.blkcnt = 0x80;
            memcpy(&dskimage[dirOffset + drm * sizeof(myDir)], &myDir, sizeof(myDir));
            memset(&myDir.pointers, 0, sizeof(myDir.pointers));
            alCount = 0;
            extent++;
            myDir.extnol = extent;
            drm++;
        }
    }
    while (!fileDone);

    fclose(fpImport);

    myDir.blkcnt = (int)ceil((alCount * dpbBLS - (dpbBLS - fileRead)) / 128.00);

    memcpy(&dskimage[dirOffset + drm * sizeof(myDir)], &myDir, sizeof(myDir));

    return 0;
}

#ifdef USE_ARCH_GLOB
void* dirLoadFile(DirDiskType diskType, const char* directory, int* size)
{
    ArchGlob* glob;
    static char filename[512];

    if (diskType == 0) {
        load_dsk_msx();
    }
    else {
        load_dsk_svi(diskType);
    }

    sprintf(filename, "%s/*", directory);

    glob = archGlob(filename, ARCH_GLOB_FILES);

    if (glob != NULL) {
        int rv;
        int i;
        for (i = 0; i < glob->count; i++) {
            char* fileName = strrchr(glob->pathVector[i], '/');
            if (fileName == NULL) {
                fileName = strrchr(glob->pathVector[i], '\\');
            }
            if (fileName == NULL) {
                continue;
            }
            fileName++;
            if (diskType == 0) {
                rv = add_single_file(fileName, directory);
            }
            else if (diskType == 4 || diskType == 5) {
                rv = add_single_file_svi(diskType, fileName, directory);
            }
            else {
                rv = add_single_file_cpm(diskType, fileName, directory);
            }

            if (rv) {
                free(dskimage);
                dskimage = NULL;
                break;
            }
        }

        archGlobFree(glob);
    }

    *size = dskimagesize;

    return dskimage;
}
#else
void* dirLoadFile(DirDiskType diskType, const char* directory, int* size)
{
	WIN32_FIND_DATA fileData;
    HANDLE hFile;
    static char filename[512];
    int success;
    int rv;

    if (diskType == 0) {
        load_dsk_msx();
    }
    else {
        load_dsk_svi(diskType);
    }

    sprintf(filename, "%s" DIR_SEPARATOR "*.*", directory);

    hFile = FindFirstFile(filename,&fileData);
    success = hFile != INVALID_HANDLE_VALUE;
    while (success) {
        if (fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
            if (diskType == 0) {
                rv = add_single_file(fileName, directory);
            }
            else if (diskType == 4 || diskType == 5) {
                rv = add_single_file_svi(diskType, fileName, directory);
            }
            else {
                rv = add_single_file_cpm(diskType, fileName, directory);
            }

            if (rv) {
                free(dskimage);
                dskimage = NULL;
                break;
            }
        }
        success = FindNextFile(hFile, &fileData);
	}
    
    if (hFile != INVALID_HANDLE_VALUE) {
        FindClose(hFile);
    }

    *size = dskimagesize;

    return dskimage;
}
#endif
