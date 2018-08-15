#pragma once

#include <memory>

#include <gdiplus.h>

class GraphicsCache
{
public:
	GraphicsCache(int windowWidth, int windowHeight, float windowScale, int fontSize,
		FontFamily *regularFontFamily, FontFamily *boldFontFamily);

	Graphics *g;
	Bitmap memoryBitmap;

	Color darkColor;
	Color grayColor;
	Color darkGrayColor;
	Color lightColor;
	Color brightColor;
	Color fadeColor;
	Color darkFadeColor;
	Color veryDarkFadeColor;

	Pen darkPen;
	Pen grayPen;
	Pen darkGrayPen;
	Pen lightPen;

	SolidBrush darkBrush;
	SolidBrush grayBrush;
	SolidBrush darkGrayBrush;
	SolidBrush lightBrush;
	SolidBrush brightBrush;
	SolidBrush fadeBrush;
	SolidBrush darkFadeBrush;
	SolidBrush veryDarkFadeBrush;

	StringFormat centeredStringFormat;
	StringFormat verticalStringFormat;
	StringFormat topRightStringFormat;
	StringFormat centeredRightStringFormat;

	Font font;
	Font fontSmall;
	Font boldFont;
	Font boldFontLarge;

	HCURSOR arrowCursor;
	HCURSOR handCursor;

	Bitmap *backgroundBitmap;
	std::shared_ptr<CachedBitmap> backgroundCachedBitmap;
};