#pragma once

#include <memory>

#include "helper.h"

class ProgressBar
{
public:
	ProgressBar(Bitmap *progressBitmap);

	void setValue(int newValue);
	void drawBar(Graphics *g, Pen *pen, Brush *brush);
	void drawText(Graphics *g, Font *font, Brush *brush);
	void invalidateProgressBar(HWND hWnd);

	RECT rect, headerTextRect, valueTextRect;
	std::wstring headerText;
	int previousValue;
	int value;
	int maxValue;
	bool draw;
private:
	std::shared_ptr<CachedBitmap> progressCachedBitmap;
	long long lastDraw;
};
