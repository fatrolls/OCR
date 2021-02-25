#include "StdAfx.h"
#include "ShapeExtract.h"
#include "FontLib.h"
#include "OCR_Interface.h"

int ProjectSetting_ExportToString = 0;
char* LastResult = NULL;

//not the best place for this function, but I'm in a rush
char* GetResultAsMultiRowString(std::list<FontExtracted*>* FontShapes)
{
	int MaxRows = 2000;
	int MaxWidth = 2000;
	int MaxFileSize = MaxRows * MaxWidth;
	char* fcontent = (char*)malloc(MaxFileSize);
	fcontent[0] = 0;
	int RowsWritten = 0;
	for (auto itr = FontShapes->begin(); itr != FontShapes->end(); itr++)
	{
		int RowBytes = 0;
		FontExtracted* fe = *itr;
		if (fe->AssignedString == NULL)
			continue;
		if (fe->MergedPrev != NULL)
			continue;
		//if we already used this shape
		float Angle = 0;
		if (fe->fss && fe->fss->fi)
			Angle = fe->fss->fi->Angle;
		if (fe->MergedNext == NULL && fe->MergedPrev == NULL)
		{
			RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "%s\n", fe->AssignedString);
			continue;
		}
		//print sequence of chars
		RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "%s", fe->AssignedString);
		fe = fe->MergedNext;
		while (fe != NULL)
		{
			RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "%s", fe->AssignedString);
			fe = fe->MergedNext;
		}
		RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "\n");
		RowsWritten++;
	}

	//flip order of text because image was upside down
	char* fcontent2 = (char*)malloc(MaxFileSize);
	int BytesWritten = 0;
	for (int i = RowsWritten - 1; i >= 0; i--)
		BytesWritten += sprintf_s(&fcontent2[BytesWritten], MaxFileSize- BytesWritten, "%s", &fcontent[i * MaxWidth]);

	free(fcontent);

	LastResult = fcontent2;

	return fcontent2;
}

LIBRARY_API char* RecognizeImageToString(const char* InputFileName, int BinarizationStrength)
{
	ProjectSetting_ExportToString = 1;
	RecognizeImage(InputFileName, BinarizationStrength);
	ProjectSetting_ExportToString = 0;
	return LastResult;
}

LIBRARY_API void FreeMemory(void* Memory)
{
	free(Memory);
	if (Memory == LastResult)
		LastResult = NULL;
}