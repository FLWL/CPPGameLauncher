#include "stdafx.h"
#include "fontloader.h"

#include <string>

bool FontLoader::loadFontIntoCollection(PrivateFontCollection *collection, int id)
{
	HRSRC res = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
	if (res)
	{
		HGLOBAL mem = LoadResource(NULL, res);
		void *data = LockResource(mem);
		size_t len = SizeofResource(NULL, res);

		Gdiplus::Status nResults = collection->AddMemoryFont(data, len);
		if (nResults == Gdiplus::Ok)
		{
			return true;
		}
	}

	return false;
}
