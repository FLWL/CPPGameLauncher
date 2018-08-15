#include "stdafx.h"

#include "constants.h"
#include "main.h"
#include "patcher.h"

ULONG_PTR gdiplusToken; // for shutting down gdi+
HANDLE hMutex; // for preventing multiple instances of the same program

bool startup()
{
	// Try to open the mutex.
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, Constants::GUID.c_str());

	// If hMutex is 0 then the mutex doesn't exist.
	if (!hMutex)
	{
		hMutex = CreateMutex(0, 0, Constants::GUID.c_str());
	}
	else
	{
		// This is a second instance. Bring the 
		// original instance to the top.
		HWND hWnd = FindWindow(Constants::GUID.c_str(), NULL);
		if (hWnd != NULL)
		{
			ShowWindow(hWnd, SW_MINIMIZE);
			ShowWindow(hWnd, SW_RESTORE);
			SetForegroundWindow(hWnd);
		}
		else
		{
			// Bringing on top failed
			throw std::logic_error("Please close any previous instances of this program and try again.");
		}

		return false;
	}

	return true;
}

void shutdown()
{
	// Exit
	GdiplusShutdown(gdiplusToken);
	if (hMutex != NULL) ReleaseMutex(hMutex);
}

// Entry point for the application
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// Check if user wants to uninstall
	if (wcscmp(lpCmdLine, L"-uninstall") == 0)
	{
		Patcher::uninstallFiles(Helper::getExecutableDirectory());
		return 0;
	}

	// Init GDI+
	CoInitialize(NULL);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (wcscmp(lpCmdLine, L"-ignoreinstances") == 0 || startup())
	{
		try
		{
			Main *main = new Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
			main->run();

			shutdown();
			delete main;
		}
		catch (const std::exception &ex)
		{
			std::string errorMsg = ex.what();
			MessageBoxA(NULL, errorMsg.c_str(), "Error!", 0);
		}
	}

	shutdown();
	return 0;
}