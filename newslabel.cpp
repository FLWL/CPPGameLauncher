#include "stdafx.h"
#include "newslabel.h"

NewsLabel::NewsLabel(Bitmap *background, Bitmap *hoveredBackground) : Button(background, hoveredBackground, background, background)
{
	verticallyCenteredStringFormat.SetLineAlignment(StringAlignmentCenter);
	bottomRightStringFormat.SetAlignment(StringAlignmentFar);
	bottomRightStringFormat.SetLineAlignment(StringAlignmentFar);
}

void NewsLabel::draw(Graphics *g, Font *titleFont, Font *contentFont, Font *dateFont, Brush *titleBrush, Brush *contentBrush, Brush *dateBrush)
{
	Button::draw(g);
	
	// Title
	g->DrawString(title.c_str(), -1, titleFont,
		RectF(position.x, position.y, position.width, position.height),
		&verticallyCenteredStringFormat, titleBrush);

	// Content
	int newsContentOffset = 0.465 * position.width;
	RectF newsLabelContentRect(position.x + newsContentOffset, position.y, position.width - newsContentOffset, position.height);
	g->DrawString(content.c_str(), -1, contentFont, newsLabelContentRect, NULL, contentBrush);
	
	// Date
	RectF dateRect = newsLabelContentRect;
	dateRect.Width -= 0.017 * position.width;
	g->DrawString(date.c_str(), -1, dateFont, dateRect, &bottomRightStringFormat, dateBrush);
}

void NewsLabel::setText(std::wstring title, std::wstring content, std::wstring date, std::wstring url)
{
	this->title = title;
	this->content = content;
	this->date = date;
	this->url = url;
}
