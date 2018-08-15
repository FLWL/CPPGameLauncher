#include "stdafx.h"
#include "button.h"

#include "pngloader.h"

Button::Button(Bitmap *normalBitmap, Bitmap *hoveredBitmap, Bitmap *pressedBitmap, Bitmap *disabledBitmap)
{
	this->position = {};
	this->normalCachedBitmap = PNGLoader::createCachedBitmap(normalBitmap);
	this->hoveredCachedBitmap = PNGLoader::createCachedBitmap(hoveredBitmap);
	this->pressedCachedBitmap = PNGLoader::createCachedBitmap(pressedBitmap);
	this->disabledCachedBitmap = PNGLoader::createCachedBitmap(disabledBitmap);
	this->hovered = false;
	this->pressed = false;
	this->clicked = false;
	this->disabled = false;
	this->extendLeftAmount = 0;
}

void Button::mousePressed(HWND hWnd, int x, int y)
{
	if (disabled)
		return;

	if (std::abs(position.x - x) < extendLeftAmount)
		x = position.x;

	if (Helper::isPointInRect(x, y, position))
	{
		// mouse clicked down on button
		if (!pressed)
		{
			// mark as pressed and invalidate
			pressed = true;
			invalidateButton(hWnd);
		}
	}
}

void Button::mouseReleased(HWND hWnd, int x, int y)
{
	if (disabled)
		return;

	if (std::abs(position.x - x) < extendLeftAmount)
		x = position.x;

	if (Helper::isPointInRect(x, y, position))
	{
		pressed = false;
		hovered = true;
		invalidateButton(hWnd);
		clicked = true;
	}
}

void Button::mouseMoved(HWND hWnd, int x, int y)
{
	if (disabled)
		return;

	if (std::abs(position.x - x) < extendLeftAmount)
		x = position.x;

	if (Helper::isPointInRect(x, y, position))
	{
		// mouse is on the button
		if (hovered)
		{
			// button is already being hovered
			// do nothing
		}
		else
		{
			// button is not hovered
			if (!pressed)
			{
				// hover if not pressed
				hovered = true;
				invalidateButton(hWnd);
			}
		}
	}
	else if (pressed || hovered)
	{
		// mouse is not on minimize button, but the button is pressed or hovered
		pressed = false;
		hovered = false;
		invalidateButton(hWnd);
	}
}

bool Button::isClicked()
{
	if (clicked)
	{
		clicked = false;
		hovered = false;
		return true;
	}

	return false;
}

bool Button::isHovered()
{
	return hovered;
}

void Button::draw(Graphics *g)
{
	g->DrawCachedBitmap(getBitmap().get(), position.x, position.y);
	//g->DrawImage(getImage(), position.x, position.y, position.width, position.height);
}

void Button::invalidateButton(HWND hWnd)
{
	RECT rect = Helper::getAbsoluteRect(position);
	InvalidateRect(hWnd, &rect, false);
}

std::shared_ptr<CachedBitmap> Button::getBitmap()
{
	if (disabled)
		return disabledCachedBitmap;
	else if (pressed)
		return pressedCachedBitmap;
	else if (hovered)
		return hoveredCachedBitmap;
	
	return normalCachedBitmap;
}
