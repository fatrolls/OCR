#include "StdAfx.h"
#include <stdio.h>

GlobalSettingsStore sSettings;

#define FILE_LINE_LENGTH	1024 * 65

void ConvertLineToParam( char *Line )
{
	if( Line[0] == '#' )
	{
//		printf("Read comment line : %s\n", Line );
		return;
	}
	char *ValueStart = strstr( Line, "=" );
	if( ValueStart != NULL && ValueStart != Line )
	{
		ValueStart++;
		char	StrValue[ FILE_LINE_LENGTH ];
		strcpy_s( StrValue, FILE_LINE_LENGTH, ValueStart );

		if (strstr(Line, "SpaceCharacterWidth"))
		{
			sSettings.SpacePixelWidth = atoi(StrValue);
			if (sSettings.SpacePixelWidth < 0)
				sSettings.SpacePixelWidth = 4;
		}
		else if( strstr( Line, "AutoAdjustSpaceWidth" ) )
			sSettings.UseDynamicSpaceWidth = atoi( StrValue );
		else if( strstr( Line, "DistanceBetweenRows" ) )
			sSettings.MinDistanceBetweenRows = atoi( StrValue );
		else if (strstr(Line, "MaxPixelCountFontDeviation"))
		{
			sSettings.MaxAllowedPixelDifferences = atoi(StrValue);
			if (sSettings.MaxAllowedPixelDifferences < 0)
				sSettings.MaxAllowedPixelDifferences = 0x7FFFFFFF;
		}
		else if( strstr( Line, "FontExactMatch" ) )
			sSettings.ExactMatchInsteadBestMatch = atoi( StrValue );
		else if( strstr( Line, "AreaScanRadius" ) )
			sSettings.AreaScan = atoi( StrValue );
		else if (strstr(Line, "FontSizeRandomness"))
			sSettings.FontSizeDeviation = atoi(StrValue);
	}
}

int ReadLine( FILE *file, char *Store, int MaxLen )
{
	int StoreIndex = 0;
	Store[ 0 ] = 0;

	char ch = getc(file);
	if( ch == EOF )
		return 0;
    while( (ch != '\n') && (ch != '\r') && (ch != EOF) && StoreIndex < MaxLen ) 
	{
        Store[ StoreIndex ] = ch;
        StoreIndex++;
        ch = getc( file );
    }
    Store[ StoreIndex++ ] = '\0';
	return StoreIndex;
}

void SetIniDefaultValues()
{
	memset(&sSettings, 0, sizeof(sSettings));
	sSettings.SpacePixelWidth = 4;
	sSettings.UseDynamicSpaceWidth = 0;
	sSettings.MinDistanceBetweenRows = FONT_HEIGHT_NEXT_ROW;
	sSettings.MaxAllowedPixelDifferences = 0x7FFFFFFF;
	sSettings.ExactMatchInsteadBestMatch = 1;
	sSettings.AreaScan = 0;
	sSettings.FontSizeDeviation = 0;
}

void LoadSettingsFromFile( const char *FileName )
{
	FILE *inf;
	if( FileName != NULL )
		errno_t err = fopen_s( &inf, FileName, "rt" );
	else
		errno_t err = fopen_s( &inf, "Config.txt", "rt" );

	if( inf )
	{
		char buff[ FILE_LINE_LENGTH ];
		int ReadRet;

		//read while comments until EOF
		while( ReadRet = ReadLine( inf, buff, FILE_LINE_LENGTH ) )
			ConvertLineToParam( buff );

		fclose( inf );
	}
}