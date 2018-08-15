#pragma once

#include <string>
#include <vector>

// A rectangle structure that uses width and height instead of absolute coordinates
struct RRECT
{
	int x;
	int y;
	int width;
	int height;
};

class Helper
{
public:
	// RRECT rectangle
	static bool isPointInRect(int x, int y, RRECT rect);
	static bool isPointInRect(int x, int y, RectF rect);
	static bool isPointInPolygon(int x, int y, std::vector<PointF> polygon);
	static RECT getAbsoluteRect(RRECT relativeRect);
	static RRECT getRelativeRect(RECT absoluteRect);

	// Files
	static std::wstring getExecutablePath();
	static std::wstring getExecutableDirectory();
	static bool fileExists(std::wstring path);
	static bool createDirectoriesForPath(std::wstring path);
	static bool executeProgram(std::wstring targetLocation);
	static bool executeProgram(std::wstring targetLocation, std::wstring params);
	static std::wstring getFileNameFromPath(std::wstring path);

	// Strings
	static std::wstring s2ws(std::string &s);
	static std::vector<std::string> split(std::string &s, char delim);
	template<typename Out>
	static void split(std::string &s, char delim, Out result);
	static int stringToInt(std::string input);
	template<class TContainer>
	static bool stringStartsWith(const TContainer &input, const TContainer &match);
	static std::wstring versionNumberToString(int latestLauncherVersion);

	// Time
	static long long currentTimeMillis();

	// Windows
	static bool GetMessageWithTimeout(MSG *msg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT timeout);
	static void flashProgramIcon(HWND hWnd);
	static bool isRunningInAdministratorMode();
};