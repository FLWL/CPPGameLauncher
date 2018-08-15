#pragma once

#include <memory>
#include <gdiplus.h>
#include <Wincodec.h>

class PNGLoader
{
public:
	static std::shared_ptr<CachedBitmap> createCachedBitmap(Bitmap *originalBitmap);
	static Bitmap *loadScaledFromResource(int id, float scale);
	static Bitmap *loadScaledFromResource(int id, float xScale, float yScale);
	static Bitmap *loadFromResource(int id);
	static Bitmap *resizeBitmap(Bitmap *source, int newWidth, int newHeight);

	static Gdiplus::Bitmap* ResizeClone(Bitmap *bmp, INT width, INT height);
private:
	static IStream *createStreamOnResource(LPCTSTR lpName, LPCTSTR lpType);
};