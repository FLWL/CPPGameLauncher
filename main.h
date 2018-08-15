#pragma once

#include <thread>
#include <list>

#include <gdiplus.h>

#include "helper.h"
#include "button.h"
#include "progressbar.h"
#include "patcher.h"
#include "newslabel.h"
#include "graphicscache.h"

#define MAX_LOADSTRING 128

class Main
{
public:
	Main(_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow);
	~Main();

	int run();
private:
	void getWindowParameters();
	void loadResources();
	void constructLayout();
	void initializeGraphics();
	static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void updateMouseCursor();
	void onLeftButtonDown(int x, int y);
	void onLeftButtonUp(int x, int y);
	void onMouseMoved(int x, int y);
	void updateScrollBarPosition(int y);
	void update();
	void paint(HDC hdc);
	void retryPatching();
	bool restartInAdministratorMode();
	RectF getAboutScrollBarButtonRect();
	void checkForPatch(bool repair);
	void parseNews();

	// Main program variables
	bool running; // whether our program is running
	HINSTANCE hInstance; // current instance
	HWND hWnd; // our window
	WCHAR szWindowTitle[MAX_LOADSTRING]; // the title bar text
	
	// Resources
	Bitmap *newsLabelBackground;
	Bitmap *newsLabelHovered;
	PrivateFontCollection normalFontCollection;
	PrivateFontCollection boldFontCollection;

	// UI elements
	Button *closeButton, *optionsButton, *minimizeButton, *playButton;
	Button *repairButton, *aboutButton;
	Button *closeAboutButton;
	Button *continueAnywaysButton, *tryAgainButton, *tryAgainAsAdminButton;
	ProgressBar *topProgressBar, *bottomProgressBar;
	NewsLabel *newsLabels[3];
	std::wstring szStatusText;
	bool graphicsCreated;
	bool isInAdminMode;
	std::vector<PointF> optionsPolygon;
	bool options;
	bool overlay, patcherOverlay, aboutOverlay;
	int currentAboutPosition;
	int aboutTextLines;
	bool draggingAboutScrollBarButton;
	int draggingOffset;

	// UI layout
	RECT windowRect;
	int screenWidth, screenHeight, refreshRate, updateRateInMs, screenDpi;
	int windowWidth, windowHeight;
	float windowScale;
	int windowX, windowY;
	int menuBarHeight, menuButtonHeight, menuButtonPadding;
	int downloadAreaHeight, downloadAreaSeparatorY;
	int playButtonPadding;
	int textHeight, logicalTextHeight, textPadding;
	int downloadBarHeight;
	RECT progressBarsRect; // area where the progress bars are located
	RECT patchNotesRect; // the area that patch notes will be written into
	RECT overlayTextRect;
	bool smallStatusText;
	std::wstring smallStatusMessage;
	
	// Patcher
	Patcher *patcher;
	std::thread patcherThread;
	int lastDownloadedBytes;
	double lastDownloadSpeed;
	long long lastDownloadSpeedTime;
	std::list<double> averageDownloadSpeedList;
	int newsLoaded;

	// Graphics
	GraphicsCache *gCache;
};