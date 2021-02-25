
1. DLL related functions
//requires a filename with 24 bpp format. If format is not 24 bpp, it will convert it to 24 bpp
//output is a string, where rows of recognized characters are separated by '\n'
LIBRARY_API char* RecognizeImageToString(const char* InputFileName, int BinarizationStrength);

//function to release memory allocated by RecognizeImageToString
LIBRARY_API void FreeMemory(void* Memory);

2. Function used internally
// Loads the image into a FIBITMAP storage. Also makes sure the freeimage DLL is loaded into memory
FIBITMAP* LoadImage_(const char* FileName);

// OCR will have a list of images with strings assigned that will be used to see if at a specific position there is a character
// by default directory from where images are loaded is "FontLib"
// images found in this directory should be black and white. White color will be treated as transparent
// image filenames should have the format : "<StringInTheImage> [anything to make filename unique].png"
// you should be using lossless compression when adding new font images to this directory
void OCR_LoadFontsFromDir(const char* Path);

// Initialize internal buffers where shape marking will be stored
// these buffers do not support multi threading !
void InitStatusStore(FIBITMAP* dib);

// perform multiple actions to detect characters and their location in an image
// step 1 : if required, do an image binarization. It is best to use an external binarization, 
// so that you can inspect visually the result and make sure the OCR input is stable
// step 2 : try to find matches of images (from fontlib) for the detected shapes
// step 3 : try to merge images found nearby to organize them in rows / columns 
// step 4 : save shapes into directory "UnrecognizedFonts" so that they can be manually renamed and moved into the "fontlib" directory
void ExtractShapesThanMatchFontsToShapes(FIBITMAP* dib, int BinarizationStrength);

//take a 24 bpp image and reduce all colors to black that are below the treshold
//non black pixels are converted to white pixels
void BinarizeImage(FIBITMAP* dib, int ConvertToBlackBelow);

// Use a floodfill algorithm to detect a shape
// shape will be marked with unique color for the sake of debugging
FontExtracted* GetShapeBoundaries(FIBITMAP* dib, int x, int y, int IDMarker);

// copy pixels from one image into a new image
void CopySourceToShape(FIBITMAP* dib, FontExtracted* sh);

// Using the images in the fontlib, try to find an exact match for the shape found at a specific location
FontSimilarityScore* GetBestMatchedFont(FIBITMAP* dib, FontExtracted* f);

//compare 2 images to see if the content ( and size ) matches exactly
int IsSameImageContent(BYTE* Pixels1, int Width1, int Height1, int pitch1, BYTE* Pixels2, int Width2, int Height2, int pitch2).
