#pragma once

#include <memory>
#include "helper.h"

class Button
{
public:
	Button(Bitmap *normalBitmap, Bitmap *hoveredBitmap, Bitmap *pressedBitmap, Bitmap *disabledBitmap);

	void mousePressed(HWND hWnd, int x, int y);
	void mouseReleased(HWND hWnd, int x, int y);
	void mouseMoved(HWND hWnd, int x, int y);

	bool isClicked();
	bool isHovered();
	void draw(Graphics *g);
	void invalidateButton(HWND hWnd);

	RRECT position;
	int extendLeftAmount;
	bool hovered;
	bool pressed;
	bool clicked;
	bool disabled;
private:
	std::shared_ptr<CachedBitmap> getBitmap();

	std::shared_ptr<CachedBitmap> normalCachedBitmap;
	std::shared_ptr<CachedBitmap> hoveredCachedBitmap;
	std::shared_ptr<CachedBitmap> pressedCachedBitmap;
	std::shared_ptr<CachedBitmap> disabledCachedBitmap;
};