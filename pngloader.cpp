#include "stdafx.h"
#include "pngloader.h"

#include "resource.h"

// Creates a cached bitmap from a regular one
std::shared_ptr<CachedBitmap> PNGLoader::createCachedBitmap(Bitmap *originalBitmap)
{
	Bitmap dummyBitmap(200, 200);
	Graphics *dummyGraphics = Graphics::FromImage(&dummyBitmap);
	return std::make_shared<CachedBitmap>(originalBitmap, dummyGraphics);
}

// Loads the PNG containing the splash image into a Bitmap of desired size.
Bitmap *PNGLoader::loadScaledFromResource(int id, float scale)
{
	return loadScaledFromResource(id, scale, scale);
}

// Loads the PNG containing the splash image into a Bitmap of desired size.
Bitmap *PNGLoader::loadScaledFromResource(int id, float xScale, float yScale)
{
	Bitmap *originalBitmap = loadFromResource(id);
	float newWidth = xScale * (float) originalBitmap->GetWidth();
	float newHeight = yScale * (float) originalBitmap->GetHeight();

	//return resizeBitmap(originalBitmap, newWidth, newHeight);
	return ResizeClone(originalBitmap, newWidth, newHeight);
}

// Loads the PNG containing the splash image into a Bitmap.
Bitmap *PNGLoader::loadFromResource(int id)
{
	// load the PNG image data into a stream
	IStream * ipImageStream = createStreamOnResource(MAKEINTRESOURCE(id), _T("PNG"));
	if (ipImageStream == NULL)
		return NULL;
	
	return Bitmap::FromStream(ipImageStream);
}

// Resizes a bitmap
Bitmap *PNGLoader::resizeBitmap(Bitmap *originalBitmap, int newWidth, int newHeight)
{
	float horizontalScalingFactor = (float)newWidth / (float)originalBitmap->GetWidth();
	float verticalScalingFactor = (float)newHeight / (float)originalBitmap->GetHeight();

	Bitmap* newBitmap = new Bitmap(newWidth, newHeight);
	Graphics imageGraphics(newBitmap);
	imageGraphics.ScaleTransform(horizontalScalingFactor, verticalScalingFactor);
	imageGraphics.DrawImage(originalBitmap, 0, 0);

	//char temp[256];
	//snprintf(temp, sizeof(temp) - 1, "resized original: %dx%d to %dx%d", originalBitmap->GetWidth(), originalBitmap->GetHeight(), newBitmap->GetWidth(), newBitmap->GetHeight());
	//MessageBoxA(NULL, temp, temp, 0);

	return newBitmap;
}

Gdiplus::Bitmap* PNGLoader::ResizeClone(Bitmap *bmp, INT width, INT height)
{
	UINT o_height = bmp->GetHeight();
	UINT o_width = bmp->GetWidth();
	INT n_width = width;
	INT n_height = height;
	double ratio = ((double)o_width) / ((double)o_height);
	if (o_width > o_height) {
		// Resize down by width
		n_height = static_cast<INT>(((double)n_width) / ratio);
	}
	else {
		n_width = static_cast<INT>(n_height * ratio);
	}
	Gdiplus::Bitmap* newBitmap = new Gdiplus::Bitmap(n_width, n_height, bmp->GetPixelFormat());
	Gdiplus::Graphics graphics(newBitmap);
	graphics.DrawImage(bmp, 0, 0, n_width, n_height);
	return newBitmap;
}

// Creates a stream object initialized with the data from an executable resource.
IStream *PNGLoader::createStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
	// initialize return value
	IStream * ipStream = NULL;

	// find the resource
	HRSRC hrsrc = FindResource(NULL, lpName, lpType);
	if (hrsrc == NULL)
		goto Return;

	// load the resource
	DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
	HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
	if (hglbImage == NULL)
		goto Return;

	// lock the resource, getting a pointer to its data
	LPVOID pvSourceResourceData = LockResource(hglbImage);
	if (pvSourceResourceData == NULL)
		goto Return;

	// allocate memory to hold the resource data
	HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if (hgblResourceData == NULL)
		goto Return;

	// get a pointer to the allocated memory
	LPVOID pvResourceData = GlobalLock(hgblResourceData);
	if (pvResourceData == NULL)
		goto FreeData;

	// copy the data from the resource to the new memory block
	CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
	GlobalUnlock(hgblResourceData);

	// create a stream on the HGLOBAL containing the data
	if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
		goto Return;

FreeData:
	// couldn't create stream; free the memory
	GlobalFree(hgblResourceData);

Return:
	// no need to unlock or free the resource
	return ipStream;
}
