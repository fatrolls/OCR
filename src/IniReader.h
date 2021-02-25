#ifndef _INIREADER_H_
#define _INIREADER_H_

struct GlobalSettingsStore
{
	int SpacePixelWidth; // number of pixels to be considered as a " " character
	int UseDynamicSpaceWidth; // sometimes space is bigger if sandwitched between large characters
	int MinDistanceBetweenRows; //number of pixels between 2 rows of characters. Depends at least on font size
	int MaxAllowedPixelDifferences; //exact match is best match, maybe allow a bit of wiggle(1-2pixel) if you must
	int ExactMatchInsteadBestMatch; //use closest possible match instead of exact match
	int AreaScan; //in some cases font will not overlay exactly ( first pixel mising) and it would reduce match greatly even if font matches
	int FontSizeDeviation;	//if binarization is not precise, font size will not be precise. If possible, set this to zero
};

void LoadSettingsFromFile( const char *FileName );

extern GlobalSettingsStore sSettings;
#endif