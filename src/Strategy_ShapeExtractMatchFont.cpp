#include "StdAfx.h"
#include "ShapeExtract.h"
#include "FontLib.h"
#include "FileWriter.h"

#define FONT_PIXEL_RATIO				10
#define ALLOWED_FONT_HEIGHT_DEVIATION	1

char* GetResultAsMultiRowString(std::list<FontExtracted*>* FontShapes);

void ExtractShapesThanMatchFontsToShapes(FIBITMAP* dib, int BinarizationStrength)
{
//	printf("Used binarization strength : %d\n", BinarizationStrength);
//	printf("Extracting characters with size : 8-30 px\n");
	//	OCR_ResizeFontsStaticSize(42, 42);

//	OCR_LoadFontsFromDir("FontsRescaled");
//	OCR_BinarizeFonts( 200 );
//	OCR_GenBlurredFonts(4);
//	OCR_GenRotatedFonts();
//	OCR_ThinFontLines();
//	OCR_ThinFontLines();
//	OCR_ThinFontLines();
#if defined( _DEBUG ) && 1
	OCR_SaveFontVisualDebug();
#endif // DEBUG

	//backup original image
//	BYTE* IMG_Dup = ImgDup(dib);

	//binarize colors
	if (BinarizationStrength == -1)
		BinarizationStrength = 100;
	BinarizationStrength += IsDemo * 500;
	BinarizeImage(dib, BinarizationStrength); // limit 155 - 180 . 110 for orange font
#ifdef _DEBUG
	SaveImagePNG(dib, "Binarized.png");
#endif

	//extract shapes
	int IDMarker = 2;
	int Width = FreeImage_GetWidth(dib);
	int Height = FreeImage_GetHeight(dib);
	BYTE* Pixels = FreeImage_GetBits(dib);
	int pitch = FreeImage_GetPitch(dib);

	FIBITMAP* Binarized_dib = FreeImage_Allocate(Width, Height, 24);
	int Binarized_pitch = FreeImage_GetPitch(Binarized_dib);
	BYTE* Binarized_Pixels = FreeImage_GetBits(Binarized_dib);
	memcpy(Binarized_Pixels, Pixels, Height * Binarized_pitch);

//	ErodeOutside(Pixels, Width, Height, pitch, 128);
#ifdef _DEBUG
//	SaveImagePNG(dib, "Eroded.png");
#endif

	std::list<FontExtracted*> FontShapes;
	for (int y = 0; y < Height; y++)
		for (int x = 0; x < Width; x++)
		{
			//check if we can detect a shape at this location
			FontExtracted* es = GetShapeBoundaries(dib, x, y, IDMarker);
			if (es == NULL)
				continue;

			IDMarker = (IDMarker * 0x007007007) & 0x00FFFFFF;

			//we expect fonts to have a pixel to size ratio of at least x%
			//chances are this is some sort of "line" / "shape"
#define EVRYTHING_IS_A_FONT
#if !defined(EVRYTHING_IS_A_FONT)
			float ShapePixelCount = (float)(es->Width * es->Height);
			float PixelRatio = es->PixelCount * 100 / ShapePixelCount;

			if (PixelRatio < FONT_PIXEL_RATIO || es->Height < 8)
			{
				free(es);
				continue;
			}
#endif

			FontShapes.push_back(es);
		}
#ifdef TRY_TO_GUESS_FONT_SIZE
	//try to guess present font heights
	//numbers should have similar heights
	//labels will probably start with [x]. The brackets should have same size
	int HeightSum = 0;
	int HeightSumCount = 0;
	int MinHeight = 100;
	int MaxHeight = 0;
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		int SimilarHeightedShapesCount = 0;
		int RefHeight = (*itr)->Height;
		int RefWidth = (*itr)->Width;
		if (RefHeight < 10)
			continue;
		if (RefHeight > 100)
			continue;
		for (auto itr2 = FontShapes.begin(); itr2 != FontShapes.end(); itr2++)
		{
			int CurHeight = (*itr2)->Height;
			int CurWidth = (*itr2)->Width;
			if (CurHeight - ALLOWED_FONT_HEIGHT_DEVIATION <= RefHeight && CurHeight + ALLOWED_FONT_HEIGHT_DEVIATION >= RefHeight
				&& CurWidth - ALLOWED_FONT_HEIGHT_DEVIATION <= RefWidth && CurWidth + ALLOWED_FONT_HEIGHT_DEVIATION >= RefWidth)
				SimilarHeightedShapesCount++;
		}
		if (SimilarHeightedShapesCount >= 2)
		{
			HeightSum += RefHeight;
			HeightSumCount++;
			if (MinHeight > RefHeight)
				MinHeight = RefHeight;
			if (MaxHeight < RefHeight)
				MaxHeight = RefHeight;
			//			printf("Possible font size : %dx%d. Found %d\n", RefHeight, RefWidth, SimilarHeightedShapesCount);
		}
		//		else
		//			printf("Non font siutable size : %dx%d\n", RefHeight, RefWidth);
	}
	int AvgSize = 0;
	if (HeightSumCount > 0)
		AvgSize = HeightSum / HeightSumCount;
	printf("FontGuessing : Avg shape size %d, min %d, max %d\n", AvgSize, MinHeight, MaxHeight);
	int AvgFontHeightOCR = OCR_GetNumbersAvgFontHeight();
	printf("FontLib : Avg number height %d\n", AvgFontHeightOCR);
	float RescaleTo = (float)AvgFontHeightOCR / (float)MinHeight;
	printf("Will try to upscale input image by %f\n", RescaleTo);
	int NewHeight = Height * RescaleTo;
	int NewWidth = Width * RescaleTo;
	FIBITMAP* NewImg = RescaleImg(dib, NewWidth, NewHeight);
#ifdef _DEBUG
	SaveImagePNG(NewImg, "Rescaled.png");
#endif
	BlurrImageToGrayScale(NewImg, 4);
#ifdef _DEBUG
	SaveImagePNG(NewImg, "RescaledBlurred.png");
#endif
#endif

#define TryToMatchShapesToFonts
#ifdef TryToMatchShapesToFonts

#define DUMP_GUESSED_CHARS_TO_NEW_IMG
#if defined(DUMP_GUESSED_CHARS_TO_NEW_IMG) && defined(_DEBUG)
	FIBITMAP* dib2 = FreeImage_Allocate(Width, Height, 24);
	int pitch2 = FreeImage_GetPitch(dib2);
	BYTE* Pixels2 = FreeImage_GetBits(dib2);
	memset(Pixels2, 255, Height * pitch2);
#endif

//	RestoreContentFromDup(dib, IMG_Dup);

//	FreeImage_Save(FIF_PNG, Binarized_dib, "beforefontmatch.png", 0);
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		FontExtracted* fe = *itr;
		if (fe->Pixels == NULL)
			CopySourceToShape(dib, fe);
		FontSimilarityScore* fss = GetBestMatchedFont(Binarized_dib, fe);
		if (fss == NULL)
			continue;
		if (fss->fi == NULL)
		{
			free(fss);
			continue;
		}
		fe->fss = fss;
#ifdef _DEBUG
		printf("At pos %dx%d found string %s\n", fe->x, fe->y, fss->fi->AssignedString);
#endif
		fe->AssignedString = _strdup(fss->fi->AssignedString);
#if defined(DUMP_GUESSED_CHARS_TO_NEW_IMG) && defined(_DEBUG)
		BYTE* ScaledFont = RescaleImg(fss->fi->Pixels, fss->fi->Width, fss->fi->Height, fss->fi->pitch, fe->Width, fe->Height);
		for (int ty = 0; ty < fe->Height; ty++)
			for (int tx = 0; tx < fe->Width; tx++)
			{
				Pixels2[(fe->y + ty) * pitch2 + (fe->x + tx) * Bytespp + 0] = ScaledFont[ty * fe->Width * Bytespp + tx * Bytespp + 0];
				Pixels2[(fe->y + ty) * pitch2 + (fe->x + tx) * Bytespp + 1] = ScaledFont[ty * fe->Width * Bytespp + tx * Bytespp + 1];
				Pixels2[(fe->y + ty) * pitch2 + (fe->x + tx) * Bytespp + 2] = ScaledFont[ty * fe->Width * Bytespp + tx * Bytespp + 2];
			}
		free(ScaledFont);
#endif
	}
#if defined(DUMP_GUESSED_CHARS_TO_NEW_IMG) && defined(_DEBUG)
	FreeImage_Save(FIF_PNG, dib2, "guessedFonts.png", 0);
#endif

	//try to merge consecutive shape texts that are from left to right
	int Progress = 0;
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		FontExtracted* fe = *itr;
		fe->LastX = fe->x + fe->Width;
		fe->LastY = fe->y;
		fe->MergedNext = NULL;
		fe->MergedPrev = NULL;
	}
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		FontExtracted* fe = *itr;
		if (fe->AssignedString == NULL)
			continue;
		//if we already used this shape
		if (fe->MergedNext != NULL || fe->MergedPrev != NULL)
			continue;
		//get all shapes on this row
		std::list<FontExtracted*> RowShapes;
		RowShapes.clear();
		for (auto itr2 = FontShapes.begin(); itr2 != FontShapes.end(); itr2++)
		{
			FontExtracted* fe2 = *itr2;
			if (fe2->AssignedString == NULL)
				continue;
			if (fe2->MergedNext != NULL || fe2->MergedPrev != NULL)
				continue;
			if (abs(fe2->y - fe->y) > sSettings.MinDistanceBetweenRows)
				continue;
			RowShapes.push_back(fe2);
		}

		//order shapes by x
		RowShapes.sort([](const FontExtracted* a, const FontExtracted* b) { return a->x < b->x; });

		//merge from left to right characters into strings
		FontExtracted* cur = NULL;
		for (auto itr2 = RowShapes.begin(); itr2 != RowShapes.end(); itr2++)
		{
			FontExtracted* next = *itr2;
			if (cur == NULL)
			{
				cur = next;
//				printf("%dx%d %s", next->x, next->y, next->AssignedString);
				continue;
			}
			if (!(cur->LastX - 5 > next->x || cur->LastX + 30 < next->x))
			{
				//maybe there is a ' ' between the 2 characters
				int EstimatedSpaceWidth = sSettings.SpacePixelWidth;
				if (sSettings.UseDynamicSpaceWidth)
				{
					int PrevFontWidth = cur->Width;
					int NextFontWidth = next->Width;
					EstimatedSpaceWidth = (PrevFontWidth + NextFontWidth + 4 * SPACE_FONT_WIDTH) / 6;
					if (EstimatedSpaceWidth < 3)
						EstimatedSpaceWidth = 4;
				}
				if (cur->LastX + EstimatedSpaceWidth <= next->x) //really depends on font width
				{
					int newlen = (int)strlen(cur->AssignedString) + 2;
					char *t = (char*)malloc(newlen);
					sprintf_s(t, newlen, "%s ", cur->AssignedString);
					free(cur->AssignedString);
					cur->AssignedString = t;
				}

				cur->MergedNext = next;
				next->MergedPrev = cur; //merged to self :P
				cur->LastX = next->LastX;
//				printf("%s", next->AssignedString);
			}
			else
			{
//				printf("\n"); //end if a string;
//				printf("%dx%d %s", next->x, next->y, next->AssignedString);
			}
			cur = next;
		}
//		printf("\n"); //end if a string;
	}
#ifdef _DEBUG
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		FontExtracted* fe = *itr;
		if (fe->AssignedString == NULL)
			continue;
		if (fe->MergedPrev != NULL)
			continue;
		//if we already used this shape
		if (fe->MergedNext == NULL && fe->MergedPrev == NULL)
		{
			printf("At %dx%d, char : %s\n", fe->x, fe->y, fe->AssignedString);
			continue;
		}
		//print sequence of chars
		printf("At %dx%d, text : %s", fe->x, fe->y, fe->AssignedString);
		fe = fe->MergedNext;
		while (fe != NULL)
		{
			printf("%s", fe->AssignedString);
			fe = fe->MergedNext;
		}
		printf("\n");
	}
#endif
#endif

#define DUMP_UNRECOGNIZED_FONTS
#if defined( DUMP_UNRECOGNIZED_FONTS )
	//save all unrecognized shapes to library
	int FileNameIndex = 1;
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		if ((*itr)->AssignedString != NULL)
			continue;
		int AlreadyDumped = 0;
		for (auto itr2 = itr; itr2 != FontShapes.begin(); itr2--)
		{
			if (*itr2 == *itr)
				continue;
			if (IsSameImageContent((*itr2)->Pixels, (*itr2)->Width, (*itr2)->Height, (*itr2)->Width * Bytespp, (*itr)->Pixels, (*itr)->Width, (*itr)->Height, (*itr)->Width * Bytespp))
			{
				AlreadyDumped = 1;
				break;
			}
		}
		if (AlreadyDumped == 1)
			continue;
		char FileName[_MAX_PATH];
		sprintf_s(FileName, sizeof(FileName), "./UnrecognizedFonts/%d.png", FileNameIndex);
		SaveShapeToPNGFile(dib, *itr, FileName);
		FileNameIndex++;
	}
#endif

	FreeImage_Unload(Binarized_dib);

#ifdef _DEBUG
	SaveImagePNG(dib, "ShapesMarked.png");
#endif
	if (ProjectSetting_ExportToString == 1)
	{
		GetResultAsMultiRowString(&FontShapes);
	}
	else
	{
		//	WriteCharsToFile(&FontShapes,"CharactersExtracted.txt");
		WriteTextToFileMerged(&FontShapes, "TextExtracted.txt");
	}
	//memory cleanup
	//should have used "new" and simply delete classes :(
	for (auto itr = FontShapes.begin(); itr != FontShapes.end(); itr++)
	{
		FontExtracted* fe = *itr;
		if (fe->Pixels)
		{
			free(fe->Pixels);
			fe->Pixels = NULL;
		}
		if (fe->AssignedString)
		{
			free(fe->AssignedString);
			fe->AssignedString = NULL;
		}
		if (fe->fss)
		{
			free(fe->fss);
			fe->fss = NULL;
		}
		free(fe);
	}
//	free(IMG_Dup);
}