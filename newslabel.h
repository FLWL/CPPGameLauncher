#pragma once

#include <gdiplus.h>

#include "button.h"

class NewsLabel : public Button
{
public:
	NewsLabel(Bitmap *background, Bitmap *hoveredBackground);

	void draw(Graphics *g, Font *titleFont, Font *contentFont, Font *dateFont, Brush *titleBrush, Brush *contentBrush, Brush *dateBrush);
	void setText(std::wstring title, std::wstring content, std::wstring date, std::wstring url);

	std::wstring url;
private:
	std::wstring title;
	std::wstring content;
	std::wstring date;

	StringFormat verticallyCenteredStringFormat;
	StringFormat bottomRightStringFormat;
};