#include "stdafx.h"
#include "graphicscache.h"

#include "resource.h"
#include "pngloader.h"

GraphicsCache::GraphicsCache(int windowWidth, int windowHeight, float windowScale, int fontSize,
	FontFamily *regularFontFamily, FontFamily *boldFontFamily)
	:
	memoryBitmap(windowWidth, windowHeight),

	darkColor(0, 0, 0),
	grayColor(255, 230, 230, 230),
	darkGrayColor(150, 255, 255, 255),
	lightColor(255, 255, 255, 255),
	brightColor(255, 255, 215, 0),
	fadeColor(76, 0, 0, 0),
	darkFadeColor(125, 0, 0, 0),
	veryDarkFadeColor(230, 0, 0, 0),

	darkPen(darkColor),
	grayPen(grayColor),
	darkGrayPen(darkGrayColor),
	lightPen(lightColor),

	darkBrush(darkColor),
	grayBrush(grayColor),
	darkGrayBrush(darkGrayColor),
	lightBrush(lightColor),
	brightBrush(brightColor),
	fadeBrush(fadeColor),
	darkFadeBrush(darkFadeColor),
	veryDarkFadeBrush(veryDarkFadeColor),

	centeredStringFormat(),
	verticalStringFormat(),
	topRightStringFormat(),
	centeredRightStringFormat(),

	font(regularFontFamily, fontSize, FontStyleRegular, UnitPixel),
	fontSmall(regularFontFamily, fontSize * 0.8, FontStyleRegular, UnitPixel),
	boldFont(boldFontFamily, fontSize, FontStyleBold, UnitPixel),
	boldFontLarge(boldFontFamily, fontSize*2.5, FontStyleBold, UnitPixel)
{
	g = Graphics::FromImage(&memoryBitmap);
	g->SetTextRenderingHint(TextRenderingHintAntiAlias);

	centeredStringFormat.SetAlignment(StringAlignmentCenter);
	centeredStringFormat.SetLineAlignment(StringAlignmentCenter);
	verticalStringFormat.SetLineAlignment(StringAlignmentCenter);
	topRightStringFormat.SetAlignment(StringAlignmentFar);
	centeredRightStringFormat.SetAlignment(StringAlignmentFar);
	centeredRightStringFormat.SetLineAlignment(StringAlignmentCenter);

	arrowCursor = LoadCursor(NULL, IDC_ARROW);
	handCursor = LoadCursor(NULL, IDC_HAND);

	// Load bitmaps, scale and cache them
	backgroundBitmap = PNGLoader::loadScaledFromResource(IDB_PNG1, windowScale);
	backgroundCachedBitmap = PNGLoader::createCachedBitmap(backgroundBitmap);
}
