#pragma once

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <cstdlib>
	#include <crtdbg.h>
#endif

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <list>
#include "IniReader.h"
#include "FreeImage.h"
#include "ImageHandler.h"
#include "ImageFilters.h"
#include "ShapeStore.h"
//#include "DetectSquare.h"
//#include "DetectLine.h"
#include "ExtractionStatusStore.h"
#include "FileWriter.h"


#ifndef MIN
	#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
	#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef ABS
	#define ABS(x) (((x)<0)?(-(x)):(x))
#endif

#define SPACE_FONT_WIDTH 4
#define FONT_HEIGHT_NEXT_ROW 10

#define SHP_FILE_VERSION_STRING "ver2"
#define Bytespp	3

extern int IsDemo;
extern int ProjectSetting_ExportToString ;
extern char* LastResult;