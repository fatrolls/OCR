#include "StdAfx.h"
#include "Strategy_ShapeExtractMatchFont.h"
#include "FontLib.h"
#include "OCR_Interface.h"
#include "IniReader.h"


int IsDemo = 0;
__forceinline void CheckSetISDemo()
{
	//get date now
	SYSTEMTIME st, lt;
	GetSystemTime(&st);
	GetLocalTime(&lt);
	if (st.wMonth == 1 && lt.wMonth == 1)
		IsDemo = 0;
}

int FontsLoaded = 0;
LIBRARY_API int RecognizeImage(const char* InputFileName, int BinarizationStrength)
{
	FIBITMAP* dib = NULL;
	dib = LoadImage_(InputFileName);

	if (dib == NULL)
	{
		printf("Could not open input file\n");
		return -1;
	}

	int bpp = FreeImage_GetBPP(dib);
	if (bpp != 24)
		dib = FreeImage_ConvertTo24Bits(dib);
	bpp = FreeImage_GetBPP(dib);
	if (bpp != 24)
	{
		printf("!!!!Only support 24 bpp input. Upgrade software or convert input from %d to 24\n", bpp);
		return -1;
	}

	if (FontsLoaded == 0)
	{
		LoadSettingsFromFile("Init.cfg");
		OCR_LoadFontsFromDir("FontLib");
		FontsLoaded = 1;
	}

	CheckSetISDemo();

	InitStatusStore(dib);

	ExtractShapesThanMatchFontsToShapes(dib, BinarizationStrength);

	// free the dib
	FreeImage_Unload(dib);

#ifndef LIBRARY_EXPORTS
	//should only call it once. Does not support multi loading
//	FreeImage_DeInitialise();
#endif

	return 0;
}

#ifndef LIBRARY_EXPORTS
int main(int argc, char** argv)
{
	// open and load the file using the default load option
	const char* InputFileName = "../Source list Box/I_12390.jpg";
//	const char* InputFileName = "../Source list Box/I_1899.jpg";
//	const char* InputFileName = "../Source list Box/bugged_.png";

	if (argc == 1)
		printf("Usage: ocr1.exe <inputFilename> [BinarizationTreshold]\n");
	if (argc > 1)
		InputFileName = argv[1];
	int BinarizationStrength = -1;
	if (argc > 2)
		BinarizationStrength = atoi(argv[2]);

	RecognizeImage(InputFileName, BinarizationStrength);
#if 0
	{
//		char *ret = RecognizeImageToString("../Reliability issue/cutout3.png", -1);
//		printf("%s\n", ret);
//		return 0;
		char TempResult1[5000];
		char TempResult2[5000];
		for (int i = 0; i < 2; i++)
		{
			char* ret;
			ret = RecognizeImageToString("../Reliability issue/workng/504.png", -1);
			strcpy_s(TempResult1, sizeof(TempResult1), ret);
//			printf("%s\n", ret);
			FreeMemory(ret);
			ret = RecognizeImageToString("../Reliability issue/not working/88.png", -1);
			strcpy_s(TempResult2, sizeof(TempResult2), ret);
//			printf("%s\n", ret);
			FreeMemory(ret);
			if (strcmp(TempResult1, TempResult2) != 0)
			{
				printf("Results mismatch :\n");
				printf("%s\n", TempResult1);
				printf("%s\n", TempResult2);
			}
		}
	}
#endif

	//cleanup
	CleanupStatusStore();
	OCR_UnloadFonts();
	FreeImage_DeInitialise();

	//check for leaked memory
	_CrtDumpMemoryLeaks();

	return 0;
}
#endif