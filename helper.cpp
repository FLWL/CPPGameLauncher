#include "stdafx.h"
#include "helper.h"

#include <sstream>
#include <vector>
#include <iterator>
#include <sys/timeb.h>

bool Helper::isPointInRect(int x, int y, RRECT rect)
{
	return (x >= rect.x && x < rect.x + rect.width && y >= rect.y && y < rect.y + rect.height);
}

bool Helper::isPointInRect(int x, int y, RectF rect)
{
	return (x >= rect.X && x < rect.X + rect.Width && y >= rect.Y && y < rect.Y + rect.Height);
}

bool Helper::isPointInPolygon(int x, int y, std::vector<PointF> polygon)
{
	// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
	bool result = false;

	int i, j;
	for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
	{
		if (((polygon[i].Y>y) != (polygon[j].Y>y)) &&
			(x < (polygon[j].X - polygon[i].X) * (y - polygon[i].Y) / (polygon[j].Y - polygon[i].Y) + polygon[i].X))
			result = !result;
	}

	return result;
}

RECT Helper::getAbsoluteRect(RRECT relativeRect)
{
	RECT r;
	r.left = relativeRect.x;
	r.top = relativeRect.y;
	r.right = relativeRect.x + relativeRect.width;
	r.bottom = relativeRect.y + relativeRect.height;
	return r;
}

RRECT Helper::getRelativeRect(RECT absoluteRect)
{
	RRECT r;
	r.x = absoluteRect.left;
	r.y = absoluteRect.top;
	r.width = absoluteRect.right - absoluteRect.left;
	r.height = absoluteRect.bottom - absoluteRect.top;
	return r;
}

std::wstring Helper::getExecutablePath()
{
	std::vector<wchar_t> pathBuf;
	DWORD copied = 0;
	do {
		pathBuf.resize(pathBuf.size() + MAX_PATH);
		copied = GetModuleFileName(0, &pathBuf.at(0), pathBuf.size());
	} while (copied >= pathBuf.size());
	pathBuf.resize(copied);

	std::wstring path = std::wstring(pathBuf.begin(), pathBuf.end());
	return path;
}

std::wstring Helper::getExecutableDirectory()
{
	std::wstring path = getExecutablePath();
	return path.substr(0, path.find_last_of(L"\\/"));
}

bool Helper::fileExists(std::wstring path)
{
	DWORD dwAttrib = GetFileAttributes(path.c_str());
	return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool Helper::createDirectoriesForPath(std::wstring path)
{
	unsigned int lastSlashPos = path.find_last_of(L"\\/");
	unsigned int pos = 0;
	do
	{
		pos = path.find_first_of(L"\\/", pos + 1);

		if (!CreateDirectory(path.substr(0, pos).c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			if (GetLastError() == ERROR_ACCESS_DENIED) // fix for putting the launcher into C:/
				continue;

			return false;
		}
	} while (pos < lastSlashPos);

	return true;
}

bool Helper::executeProgram(std::wstring targetLocation)
{
	return executeProgram(targetLocation, L"");
}

bool Helper::executeProgram(std::wstring targetLocation, std::wstring params)
{
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpVerb = NULL;
	sei.lpFile = targetLocation.c_str();
	sei.lpParameters = params.c_str();
	sei.hwnd = NULL;
	sei.nShow = SW_SHOWDEFAULT;

	return ShellExecuteEx(&sei);
}

std::wstring Helper::getFileNameFromPath(std::wstring path)
{
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	_wsplitpath_s(path.c_str(), drive, dir, fname, ext);

	return std::wstring(fname) + std::wstring(ext);
}

std::wstring Helper::s2ws(std::string &s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::vector<std::string> Helper::split(std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

template<typename Out>
void Helper::split(std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (!item.empty())
			*(result++) = item;
	}
}

int Helper::stringToInt(std::string input)
{
	int output = 0;

	try
	{
		output = std::stoi(input);
	}
	catch (const std::exception &ex)
	{
		output = 0;
	}

	return output;
}

template<class TContainer>
bool Helper::stringStartsWith(const TContainer& input, const TContainer& match)
{
	return input.size() >= match.size()
		&& equal(match.begin(), match.end(), input.begin());
}

std::wstring Helper::versionNumberToString(int latestLauncherVersion)
{
	wchar_t temp[16];
	_snwprintf(temp, sizeof(temp) - 1, L"%.2f", (latestLauncherVersion / 100.0f));

	return std::wstring(temp);
}

long long Helper::currentTimeMillis()
{
	_timeb t;
	_ftime64_s(&t);
	return t.time * 1000LL + t.millitm;
}

bool Helper::GetMessageWithTimeout(MSG *msg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT timeout)
{
	BOOL res;
	UINT_PTR timerId = SetTimer(NULL, 0, timeout, NULL);
	res = GetMessage(msg, hWnd, wMsgFilterMin, wMsgFilterMax);
	KillTimer(NULL, timerId);

	if (!res)
		return false;
	if (msg->message == WM_TIMER && msg->hwnd == NULL && msg->wParam == timerId)
		return false; // timed out

	return true;
}

void Helper::flashProgramIcon(HWND hWnd)
{
	FLASHWINFO fi;
	fi.cbSize = sizeof(FLASHWINFO);
	fi.hwnd = hWnd;
	fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fi.uCount = 0;
	fi.dwTimeout = 0;
	FlashWindowEx(&fi);
}

bool Helper::isRunningInAdministratorMode()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}