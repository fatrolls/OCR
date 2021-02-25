#pragma once

#if defined(LIBRARY_EXPORTS) || defined(_CONSOLE)
#    define LIBRARY_API __declspec(dllexport)
#else
#    define LIBRARY_API __declspec(dllimport)
#endif

LIBRARY_API int RecognizeImage(const char* InputFileName, int BinarizationStrength);

LIBRARY_API char *RecognizeImageToString(const char* InputFileName, int BinarizationStrength);
LIBRARY_API void FreeMemory(void *Memory);
