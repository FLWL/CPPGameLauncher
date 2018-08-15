#pragma once

#include <gdiplus.h>

class FontLoader
{
public:
	static bool loadFontIntoCollection(PrivateFontCollection *collection, int id);
};