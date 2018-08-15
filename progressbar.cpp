#include "stdafx.h"
#include "progressbar.h"

#include "pngloader.h"

ProgressBar::ProgressBar(Bitmap *progressBitmap)
{
	this->progressCachedBitmap = PNGLoader::createCachedBitmap(progressBitmap);
	this->value = 0;
	this->maxValue = 100;
	this->headerText = L"Loading...";
	this->draw = false;
}

void ProgressBar::setValue(int newValue)
{
	previousValue = value;
	value = newValue;
}

void ProgressBar::drawBar(Graphics *g, Pen *pen, Brush *brush)
{
	if (!draw)
		return;

	int progressBarHeight = rect.bottom - rect.top;

	RRECT progressPosition;
	progressPosition.x = rect.left;
	progressPosition.y = rect.top;
	progressPosition.height = abs(rect.bottom - rect.top);

	float fractionalProgress = (float)value / (float)maxValue;
	if (fractionalProgress > 0.995)
		fractionalProgress = 0.995;

	int totalBarWidth = rect.right - rect.left - progressBarHeight * 0.1;

	int currentBarWidth = (int) ((float) totalBarWidth * fractionalProgress);
	if (currentBarWidth > totalBarWidth)
		currentBarWidth = totalBarWidth;
	progressPosition.width = currentBarWidth;

	int indicatorX = (int) (rect.left + currentBarWidth - (progressBarHeight / 2.35f));

	// Progress indicator
	g->DrawCachedBitmap(progressCachedBitmap.get(), indicatorX, rect.top);

	//char temp[256];
	//snprintf(temp, sizeof(temp) - 1, "value = %d, maxValue = %d, rect.left = %d, currentBarWidth = %d, progress = %f, bar X = %d", value, maxValue, rect.left, currentBarWidth, fractionalProgress, indicatorX);
	//MessageBoxA(NULL, temp, temp, 0);
	
	// Background
	g->FillRectangle(brush, Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top));
	lastDraw = Helper::currentTimeMillis();
}

void ProgressBar::drawText(Graphics *g, Font *font, Brush *brush)
{
	if (!draw)
		return;

	int padding = (rect.right - rect.left) * 0.02;
	RectF layout = RectF(rect.left + padding, rect.top, rect.right - rect.left - 2*padding, rect.bottom - rect.top);
	StringFormat sf(StringFormatFlagsNoWrap);
	sf.SetLineAlignment(StringAlignmentCenter);

	PointF point(headerTextRect.left, headerTextRect.top);
	g->DrawString(headerText.c_str(), -1, font, layout, &sf, brush);

	int percentage = 0;
	if (maxValue != 0)
		percentage = (float)value / (float)maxValue * 100.0f;
	
	wchar_t szPercentage[8];
	std::swprintf(szPercentage, sizeof(szPercentage) / sizeof(*szPercentage), L"%d%%", percentage);

	StringFormat sfCentered(StringFormatFlagsNoWrap);
	sfCentered.SetAlignment(StringAlignmentFar);
	sfCentered.SetLineAlignment(StringAlignmentCenter);
	g->DrawString(szPercentage, -1, font, layout, &sfCentered, brush);
}

void ProgressBar::invalidateProgressBar(HWND hWnd)
{
	int progressBarHeight = rect.bottom - rect.top;

	RECT r;
	r.top = rect.top;
	r.left = rect.left - 0.25 * progressBarHeight;
	r.bottom = rect.bottom;
	r.right = rect.right + 0.25 * progressBarHeight;
	InvalidateRect(hWnd, &r, true);
}