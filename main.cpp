#include "stdafx.h"
#include "main.h"

#include <Windowsx.h>
#include <sstream>

#define CURL_STATICLIB
#include <curl/curl.h>

#include "resource.h"
#include "constants.h"
#include "downloader.h"
#include "pngloader.h"
#include "fontloader.h"

// Start up a Windows GUI application
Main::Main(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// Assign global variables
	this->hInstance = hInstance;

	// Init curl once
	curl_global_init(CURL_GLOBAL_ALL);
	// Init patcher
	patcher = new Patcher();

	getWindowParameters();
	loadResources();
	constructLayout();
	initializeGraphics();

	// Register the window class
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = _WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CPP_GAME_LAUNCHER);
	wcex.lpszClassName = Constants::GUID.c_str();
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex);

	// Create window
	hWnd = CreateWindowExW(0, Constants::GUID.c_str(), szWindowTitle, WS_POPUP | WS_MINIMIZEBOX,
		windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, nullptr, nullptr);

	if (!hWnd)
		throw std::logic_error("Failed to create window!");

	// Save a pointer to our current class to call Main::WndProc from static function _WndProc
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (long) this);
	// Define window region we are drawing in
	HRGN hRgn = CreateRectRgn(0, 0, windowWidth, windowHeight);
	SetWindowRgn(hWnd, hRgn, TRUE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	running = true;
}
Main::~Main()
{
	std::exit(0); // terminate the patcher thread
}

void Main::getWindowParameters()
{
	// Refresh rate
	DEVMODE lpDevMode;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &lpDevMode);
	refreshRate = lpDevMode.dmDisplayFrequency;
	if (refreshRate < 30 || refreshRate > 240)
	{
		refreshRate = 60;
	}
	updateRateInMs = 1000.0 / refreshRate;

	// DPI
	HDC screenHdc = GetDC(0);
	screenDpi = GetDeviceCaps(screenHdc, LOGPIXELSY);
	ReleaseDC(0, screenHdc);

	// Window size
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	windowHeight = screenHeight * 0.5;
	if (windowHeight < 480) windowHeight = 480;
	windowWidth = windowHeight * (16.0 / 9.0);
	windowScale = windowHeight / 1080.0f;
	windowX = (screenWidth - windowWidth) * 0.5;
	windowY = (screenHeight - windowHeight) * 0.5;

	// Admin mode
	isInAdminMode = Helper::isRunningInAdministratorMode();
}

void Main::loadResources()
{
	// Strings
	LoadStringW(hInstance, IDS_APP_TITLE, szWindowTitle, MAX_LOADSTRING);

	// Images
	newsLabelBackground = PNGLoader::loadScaledFromResource(IDB_PNG9, windowScale);
	newsLabelHovered = PNGLoader::loadScaledFromResource(IDB_PNG29, windowScale);

	// Buttons
	closeButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG2, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG3, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG4, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG2, windowScale));
	optionsButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG8, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG15, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG16, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG16, windowScale));
	minimizeButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG5, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG6, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG7, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG5, windowScale));
	playButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG11, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG12, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG14, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG13, windowScale));
	playButton->disabled = true;
	// Options buttons
	repairButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG17, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG18, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG19, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG19, windowScale));
	repairButton->disabled = true;
	aboutButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG20, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG21, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG22, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG22, windowScale));
	// About buttons
	closeAboutButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG23, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG24, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG25, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG25, windowScale));
	// Patcher error buttons
	continueAnywaysButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG23, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG24, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG25, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG25, windowScale));
	tryAgainButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG26, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG27, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG28, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG28, windowScale));
	tryAgainAsAdminButton = new Button(PNGLoader::loadScaledFromResource(IDB_PNG26, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG27, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG28, windowScale),
		PNGLoader::loadScaledFromResource(IDB_PNG28, windowScale));

	// Progress bars
	topProgressBar = new ProgressBar(PNGLoader::loadScaledFromResource(IDB_PNG10, windowScale));
	bottomProgressBar = new ProgressBar(PNGLoader::loadScaledFromResource(IDB_PNG10, windowScale));

	// Fonts
	FontLoader::loadFontIntoCollection(&normalFontCollection, IDR_RCDATA1);
	FontLoader::loadFontIntoCollection(&boldFontCollection, IDR_RCDATA2);
}

int Main::run()
{
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPP_GAME_LAUNCHER));

	// Start doing our work (in another thread)
	checkForPatch(false);

	// Main message loop:
	MSG msg;
	while (running)
	{
		update();

		while (Helper::GetMessageWithTimeout(&msg, nullptr, 0, 0, updateRateInMs))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int) msg.wParam;
}

void Main::constructLayout()
{
	// Window rect
	windowRect.top = 0;
	windowRect.left = 0;
	windowRect.right = windowWidth;
	windowRect.bottom = windowHeight;

	// Text height
	textHeight = windowHeight * 0.03;
	logicalTextHeight = abs((textHeight * 96) / 72); // dpi fixed at 96, works well on hi res displays
	textPadding = textHeight * 0.35;

	// Menu bar size
	menuBarHeight = windowHeight * 0.056;
	menuButtonHeight = menuBarHeight * 1.0;
	menuButtonPadding = menuBarHeight * 0.0;
	
	// Close window button
	closeButton->position.x = windowWidth - menuButtonHeight;
	closeButton->position.y = 0;
	closeButton->position.width = menuButtonHeight;
	closeButton->position.height = menuButtonHeight; // buttons are square
	
	// Options button
	optionsButton->position.x = closeButton->position.x - menuButtonHeight;
	optionsButton->position.y = 1;
	optionsButton->position.width = menuButtonHeight;
	optionsButton->position.height = menuButtonHeight;

	// Minimize window button
	minimizeButton->position.x = optionsButton->position.x - menuButtonHeight;
	minimizeButton->position.y = 0;
	minimizeButton->position.width = menuButtonHeight;
	minimizeButton->position.height = menuButtonHeight;

	// Separator for news and download status
	downloadAreaHeight = windowHeight * 0.26;
	downloadAreaSeparatorY = windowHeight - downloadAreaHeight;

	// Play button
	playButtonPadding = 0.1 * downloadAreaHeight;
	playButton->position.height = downloadAreaHeight - playButtonPadding;
	playButton->position.width = playButton->position.height * (16.0 / 9.0);
	playButton->position.x = windowWidth - playButtonPadding - playButton->position.width;
	playButton->position.y = downloadAreaSeparatorY;

	// News labels
	newsLoaded = 0;
	int newsLabelHeight = (windowHeight - downloadAreaHeight - menuBarHeight - 4 * playButtonPadding) / 3;
	int newsLabelWidth = newsLabelHeight * 3.26923076923;
	for (int i = 0; i < 3; i++)
	{
		newsLabels[i] = new NewsLabel(newsLabelBackground, newsLabelHovered);
		newsLabels[i]->position.width = newsLabelWidth;
		newsLabels[i]->position.height = newsLabelHeight;
		newsLabels[i]->position.x = playButtonPadding;
		newsLabels[i]->position.y = 2 + menuBarHeight + playButtonPadding + i * playButtonPadding + i * newsLabelHeight;
		newsLabels[i]->disabled = true;
	}

	// Download bar height
	downloadBarHeight = (downloadAreaHeight - 2 * playButtonPadding) / 2;

	// Bottom download bar
	bottomProgressBar->rect.bottom = windowHeight - playButtonPadding;
	bottomProgressBar->rect.top = bottomProgressBar->rect.bottom - downloadBarHeight;
	bottomProgressBar->rect.left = 0 + playButtonPadding;
	bottomProgressBar->rect.right = playButton->position.x - playButtonPadding;

	// Bottom download bar header & value text
	bottomProgressBar->headerTextRect.bottom = bottomProgressBar->rect.top - textPadding;
	bottomProgressBar->headerTextRect.top = bottomProgressBar->headerTextRect.bottom - textHeight - 11;
	bottomProgressBar->headerTextRect.left = 0 + playButtonPadding - windowHeight / 540 - 3;
	bottomProgressBar->headerTextRect.right = playButton->position.x - playButtonPadding;
	bottomProgressBar->valueTextRect.bottom = bottomProgressBar->rect.bottom - textPadding;
	bottomProgressBar->valueTextRect.top = bottomProgressBar->rect.top + textPadding;
	bottomProgressBar->valueTextRect.left = bottomProgressBar->rect.left; // same
	bottomProgressBar->valueTextRect.right = bottomProgressBar->rect.right; // same

	// Top download bar header text
	topProgressBar->headerTextRect.top = downloadAreaSeparatorY;
	topProgressBar->headerTextRect.bottom = topProgressBar->headerTextRect.top + textHeight*1.7;
	topProgressBar->headerTextRect.left = 0 + playButtonPadding - windowHeight / 540 - 3;
	topProgressBar->headerTextRect.right = playButton->position.x - playButtonPadding;

	// Top download bar
	topProgressBar->rect.top = downloadAreaSeparatorY;
	topProgressBar->rect.bottom = downloadAreaSeparatorY + downloadBarHeight;
	topProgressBar->rect.left = 0 + playButtonPadding;
	topProgressBar->rect.right = playButton->position.x - playButtonPadding;

	// Top download bar value text
	topProgressBar->valueTextRect.bottom = topProgressBar->rect.bottom - textPadding;
	topProgressBar->valueTextRect.top = topProgressBar->rect.top + textPadding;
	topProgressBar->valueTextRect.left = topProgressBar->rect.left; // same
	topProgressBar->valueTextRect.right = topProgressBar->rect.right; // same

	// Progress bars rect (where both fit into)
	int progressBarHeight = topProgressBar->rect.bottom - topProgressBar->rect.top;
	progressBarsRect.top = topProgressBar->rect.top;
	progressBarsRect.left = topProgressBar->rect.left - 0.25 * progressBarHeight;
	progressBarsRect.bottom = bottomProgressBar->rect.bottom;
	progressBarsRect.right = bottomProgressBar->rect.right + 0.25 * progressBarHeight;

	// Patch notes
	patchNotesRect.left = 0;
	patchNotesRect.top = menuBarHeight;
	patchNotesRect.right = windowWidth;
	patchNotesRect.bottom = downloadAreaSeparatorY;

	// Options
	int optionsAreaHeight = 2 * menuButtonHeight;
	PointF triangleTip(optionsButton->position.x + menuButtonHeight / 2 + 1, menuBarHeight + menuButtonHeight / 3);
	PointF triangleRight(triangleTip.X + menuButtonHeight / 2, triangleTip.Y + menuButtonHeight / 2);
	PointF topRightCorner(windowWidth - playButtonPadding, triangleRight.Y);
	PointF bottomRightCorner(windowWidth - playButtonPadding, topRightCorner.Y + optionsAreaHeight);
	PointF bottomLeftCorner(bottomRightCorner.X - 0.25 * windowWidth, bottomRightCorner.Y);
	PointF topLeftCorner(bottomLeftCorner.X, triangleRight.Y);
	PointF triangleLeft(triangleRight.X - menuButtonHeight, triangleRight.Y);
	optionsPolygon.push_back(triangleTip);
	optionsPolygon.push_back(triangleRight);
	optionsPolygon.push_back(topRightCorner);
	optionsPolygon.push_back(bottomRightCorner);
	optionsPolygon.push_back(bottomLeftCorner);
	optionsPolygon.push_back(topLeftCorner);
	optionsPolygon.push_back(triangleLeft);
	// Options -> Repair button
	repairButton->position.height = menuButtonHeight;
	repairButton->position.width = topRightCorner.X - topLeftCorner.X;
	repairButton->position.x = topLeftCorner.X;
	repairButton->position.y = topLeftCorner.Y;
	// Options -> About button
	aboutButton->position.height = menuButtonHeight;
	aboutButton->position.width = topRightCorner.X - topLeftCorner.X;
	aboutButton->position.x = topLeftCorner.X;
	aboutButton->position.y = topLeftCorner.Y + menuButtonHeight;
	// Overlay rect
	overlayTextRect.top = menuBarHeight;
	overlayTextRect.left = menuBarHeight;
	overlayTextRect.right = windowWidth - menuButtonHeight;
	overlayTextRect.bottom = windowHeight - 2 * menuBarHeight;
	// Close about overlay button
	closeAboutButton->position.height = menuButtonHeight;
	closeAboutButton->position.width = menuButtonHeight;
	closeAboutButton->position.x = overlayTextRect.right - 0.75 * menuButtonHeight;
	closeAboutButton->position.y = overlayTextRect.bottom + 0.5 * menuButtonHeight;
	closeAboutButton->extendLeftAmount = 0.1 * windowWidth;
	// Patcher error buttons
	continueAnywaysButton->position.height = menuButtonHeight;
	continueAnywaysButton->position.width = menuButtonHeight;
	continueAnywaysButton->position.x = overlayTextRect.right - 0.75 * menuButtonHeight;
	continueAnywaysButton->position.y = overlayTextRect.bottom + 0.5 * menuButtonHeight;
	continueAnywaysButton->extendLeftAmount = 0.18 * windowWidth;
	// Try again
	tryAgainButton->position.height = menuButtonHeight;
	tryAgainButton->position.width = menuButtonHeight + 0.4 * windowWidth;
	tryAgainButton->position.x = menuButtonHeight * 0.8;
	tryAgainButton->position.y = windowHeight - 1.5 * menuButtonHeight;
	// Try again as admin
	tryAgainAsAdminButton->position.height = menuButtonHeight;
	tryAgainAsAdminButton->position.width = menuButtonHeight + 0.4 * windowWidth;
	tryAgainAsAdminButton->position.x = menuButtonHeight * 0.8;
	tryAgainAsAdminButton->position.y = windowHeight - 2.7 * menuButtonHeight;
	tryAgainAsAdminButton->disabled = isInAdminMode;

	// Starting stuff
	szStatusText = L"Initializing...";
	overlay = false;
	patcherOverlay = false;
	aboutOverlay = false;
	options = false;
	smallStatusText = false;
	smallStatusMessage = L"";
}

void Main::initializeGraphics()
{
	// Custom fonts
	FontFamily fontFamily;
	int nNumFound = 0;
	normalFontCollection.GetFamilies(1, &fontFamily, &nNumFound);
	if (nNumFound == 0)
		throw std::logic_error("No font!");

	FontFamily fontFamilyBold;
	nNumFound = 0;
	boldFontCollection.GetFamilies(1, &fontFamilyBold, &nNumFound);
	if (nNumFound == 0)
		throw std::logic_error("No bold font!");

	gCache = new GraphicsCache(windowWidth, windowHeight, windowScale, logicalTextHeight, &fontFamily, &fontFamilyBold);
	graphicsCreated = false;
}

// A static function that retrieves a pointer to our class and calls the true WndProc callback
LRESULT CALLBACK Main::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Main *me = (Main *)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (me)
		return me->WndProc(hWnd, message, wParam, lParam);

	return DefWindowProc(hWnd, message, wParam, lParam);
}
// Process messages for the main window
LRESULT CALLBACK Main::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Calculate mouse coordinates relative to the top-left corner of our window
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	int windowX = windowRect.left;
	int windowY = windowRect.top;
	DWORD messagePos = GetMessagePos();
	int messageX = GET_X_LPARAM(messagePos) - windowX;
	int messageY = GET_Y_LPARAM(messagePos) - windowY;

	switch (message)
	{
		case WM_SETCURSOR:
		{
			updateMouseCursor();
			break;
		}
		case WM_LBUTTONDOWN:
		{
			onLeftButtonDown(messageX, messageY);
			break;
		}
		case WM_LBUTTONUP:
		{
			onLeftButtonUp(messageX, messageY);
			break;
		}
		case WM_MOUSEMOVE:
		{
			onMouseMoved(messageX, messageY);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			if (aboutOverlay)
			{
				int direction = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -2 : 2;
				currentAboutPosition += direction;
				if (currentAboutPosition < 0)
					currentAboutPosition = 0;
				if (currentAboutPosition > aboutTextLines - Constants::LINES_ON_ABOUT_SCREEN)
					currentAboutPosition = aboutTextLines - Constants::LINES_ON_ABOUT_SCREEN;

				RECT invalidateArea;
				invalidateArea = overlayTextRect;
				invalidateArea.right = windowWidth;
				InvalidateRect(hWnd, &invalidateArea, true);
			}

			break;
		}
		case WM_ERASEBKGND:
		{
			return 1; // Suppress window erasing to reduce flickering
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			paint(hdc);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			running = false;
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void Main::updateMouseCursor()
{
	if (overlay)
	{
		SetCursor(gCache->arrowCursor);
		return;
	}

	bool linkHovered = false;
	for (int i = 0; i < newsLoaded; i++)
	{
		if (newsLabels[i]->isHovered())
		{
			linkHovered = true;
			break;
		}
	}

	SetCursor(linkHovered ? gCache->handCursor : gCache->arrowCursor);
}

void Main::onLeftButtonDown(int x, int y)
{
	// for dragging the window
	if (y < menuBarHeight && x < (overlay ? windowWidth : (minimizeButton->position.x - menuButtonPadding)))
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)0);

	if (overlay)
	{
		if (aboutOverlay)
		{
			closeAboutButton->mousePressed(hWnd, x, y);

			RectF scrollBarButtonRect = getAboutScrollBarButtonRect();
			if (Helper::isPointInRect(x, y, scrollBarButtonRect))
			{
				draggingAboutScrollBarButton = true;
				draggingOffset = y - scrollBarButtonRect.Y;
			}
		}
		else if (patcherOverlay)
		{
			continueAnywaysButton->mousePressed(hWnd, x, y);
			tryAgainButton->mousePressed(hWnd, x, y);
			tryAgainAsAdminButton->mousePressed(hWnd, x, y);
		}

		return;
	}

	closeButton->mousePressed(hWnd, x, y);
	optionsButton->mousePressed(hWnd, x, y);
	minimizeButton->mousePressed(hWnd, x, y);
	playButton->mousePressed(hWnd, x, y);
	for (int i = 0; i < 3; i++)
		newsLabels[i]->mousePressed(hWnd, x, y);

	if (options)
	{
		repairButton->mousePressed(hWnd, x, y);
		aboutButton->mousePressed(hWnd, x, y);
	}
}

void Main::onLeftButtonUp(int x, int y)
{
	if (overlay)
	{
		if (aboutOverlay)
		{
			closeAboutButton->mouseReleased(hWnd, x, y);

			if (!draggingAboutScrollBarButton)
			{
				RectF scrollBarRect;
				scrollBarRect.X = overlayTextRect.right;
				scrollBarRect.Y = overlayTextRect.top;
				scrollBarRect.Width = 0.01 * windowWidth;
				scrollBarRect.Height = overlayTextRect.bottom - overlayTextRect.top;

				if (Helper::isPointInRect(x, y, scrollBarRect))
				{
					draggingOffset = 0;
					updateScrollBarPosition(y);
				}
			}
			else
			{
				draggingAboutScrollBarButton = false;
			}
		}
		else if (patcherOverlay)
		{
			continueAnywaysButton->mouseReleased(hWnd, x, y);
			tryAgainButton->mouseReleased(hWnd, x, y);
			tryAgainAsAdminButton->mouseReleased(hWnd, x, y);
		}

		return;
	}

	closeButton->mouseReleased(hWnd, x, y);
	optionsButton->mouseReleased(hWnd, x, y);
	minimizeButton->mouseReleased(hWnd, x, y);
	playButton->mouseReleased(hWnd, x, y);
	for (int i = 0; i < 3; i++)
		newsLabels[i]->mouseReleased(hWnd, x, y);

	if (!optionsButton->clicked && !Helper::isPointInPolygon(x, y, optionsPolygon))
	{
		// User clicked outside the options polygon while options were open
		options = false;
		InvalidateRect(hWnd, &patchNotesRect, true);
	}

	if (options)
	{
		repairButton->mouseReleased(hWnd, x, y);
		aboutButton->mouseReleased(hWnd, x, y);
	}
}

void Main::onMouseMoved(int x, int y)
{
	if (overlay)
	{
		if (aboutOverlay)
		{
			closeAboutButton->mouseMoved(hWnd, x, y);

			if (draggingAboutScrollBarButton)
			{
				updateScrollBarPosition(y);
			}
		}
		else if (patcherOverlay)
		{
			continueAnywaysButton->mouseMoved(hWnd, x, y);
			tryAgainButton->mouseMoved(hWnd, x, y);
			tryAgainAsAdminButton->mouseMoved(hWnd, x, y);
		}

		return;
	}

	closeButton->mouseMoved(hWnd, x, y);
	optionsButton->mouseMoved(hWnd, x, y);
	minimizeButton->mouseMoved(hWnd, x, y);
	playButton->mouseMoved(hWnd, x, y);
	for (int i = 0; i < 3; i++)
		newsLabels[i]->mouseMoved(hWnd, x, y);

	if (options)
	{
		repairButton->mouseMoved(hWnd, x, y);
		aboutButton->mouseMoved(hWnd, x, y);
	}
}

void Main::updateScrollBarPosition(int y)
{
	int clampedY = y - draggingOffset;
	if (clampedY < overlayTextRect.top)
		clampedY = overlayTextRect.top;
	if (clampedY > overlayTextRect.bottom)
		clampedY = overlayTextRect.bottom;

	int maxHeight = overlayTextRect.bottom - overlayTextRect.top;
	int currentHeight = clampedY - overlayTextRect.top;
	float ratio = (float)currentHeight / (float)maxHeight;
	currentAboutPosition = (int)((float)aboutTextLines * ratio);

	if (currentAboutPosition < 0)
		currentAboutPosition = 0;
	if (currentAboutPosition > aboutTextLines - Constants::LINES_ON_ABOUT_SCREEN)
		currentAboutPosition = aboutTextLines - Constants::LINES_ON_ABOUT_SCREEN;

	RECT invalidateArea;
	invalidateArea = overlayTextRect;
	invalidateArea.right = windowWidth;
	InvalidateRect(hWnd, &invalidateArea, true);
}

void Main::update()
{
	// Fix for buttons remaining hovered even after mouse has left the window
	POINT cursorPos;
	if (GetCursorPos(&cursorPos))
	{
		if (ScreenToClient(hWnd, &cursorPos))
		{
			if (!Helper::isPointInRect(cursorPos.x, cursorPos.y, Helper::getRelativeRect(windowRect)))
			{
				// Mouse is not inside window
				closeButton->mouseMoved(hWnd, -1, -1);
				optionsButton->mouseMoved(hWnd, -1, -1);
				minimizeButton->mouseMoved(hWnd, -1, -1);
				playButton->mouseMoved(hWnd, -1, -1);

				if (options)
				{
					repairButton->mouseMoved(hWnd, -1, -1);
					aboutButton->mouseMoved(hWnd, -1, -1);
				}

				if (aboutOverlay)
				{
					closeAboutButton->mouseMoved(hWnd, -1, -1);
					if (draggingAboutScrollBarButton)
					{
						if ((GetKeyState(VK_LBUTTON) & 0x80) == 0) // mouse1 not pressed
							draggingAboutScrollBarButton = false;
						else
							updateScrollBarPosition(cursorPos.y);
					}
				}
				else if (patcherOverlay)
				{
					continueAnywaysButton->mouseMoved(hWnd, -1, -1);
					tryAgainButton->mouseMoved(hWnd, -1, -1);
					tryAgainAsAdminButton->mouseMoved(hWnd, -1, -1);
				}
			}
		}
	}

	if (overlay)
	{
		if (aboutOverlay)
		{
			if (closeAboutButton->isClicked())
			{
				aboutOverlay = false;
				if (!patcherOverlay) overlay = false;
				InvalidateRect(hWnd, &windowRect, true);
			}
		}
		else if (patcherOverlay)
		{
			if (continueAnywaysButton->isClicked())
			{
				patcher->continueAfterPause = true;
				patcher->paused = false;
				patcherOverlay = false;
				if (!aboutOverlay) overlay = false;
				InvalidateRect(hWnd, &windowRect, true);
			}
			else if (tryAgainButton->isClicked())
			{
				patcher->paused = false;
				patcherOverlay = false;
				if (!aboutOverlay) overlay = false;
				InvalidateRect(hWnd, &windowRect, true);
				retryPatching();
			}
			else if (tryAgainAsAdminButton->isClicked())
			{
				restartInAdministratorMode();
			}
		}

		return;
	}
	else if (patcher->paused)
	{
		overlay = true;
		patcherOverlay = true;
		currentAboutPosition = 0;
		InvalidateRect(hWnd, &windowRect, true);
		return;
	}

	// Patcher has loaded news, but we don't have any?
	if (!newsLoaded && !patcher->newsFile.empty())
		parseNews();

	// Check if user wants to read some patch notes
	for (int i = 0; i < newsLoaded; i++)
		if (newsLabels[i]->isClicked())
			ShellExecute(0, 0, newsLabels[i]->url.c_str(), 0, 0, SW_SHOW);

	// Patcher is finished, enable play button
	if (playButton->disabled && patcher->state >= Patcher::STATE_SUCCESS)
	{
		playButton->disabled = false;
		playButton->invalidateButton(hWnd);
		Helper::flashProgramIcon(hWnd);

		repairButton->disabled = false;
		if (options) repairButton->invalidateButton(hWnd);
	}

	// Closed
	if (closeButton->isClicked())
		running = false;

	// Options clicked
	if (optionsButton->isClicked())
	{
		options = !options;
		InvalidateRect(hWnd, &patchNotesRect, true);
	}

	// Minimized
	if (minimizeButton->isClicked())
	{
		ShowWindow(hWnd, SW_MINIMIZE);
		UpdateWindow(hWnd);
	}

	// Play
	if (playButton->isClicked())
	{
		std::wstring workingDirectory = Helper::getExecutableDirectory();
		if (Helper::executeProgram(workingDirectory + L"\\" + Constants::GAME_EXECUTABLE_NAME))
		{
			running = false;
		}
	}

	if (repairButton->isClicked())
	{
		checkForPatch(true);
		options = false;
		InvalidateRect(hWnd, &patchNotesRect, true);
	}

	if (aboutButton->isClicked())
	{
		options = false;
		overlay = true;
		aboutOverlay = true;
		currentAboutPosition = 0;
		draggingAboutScrollBarButton = false;
		aboutTextLines = 100;
		InvalidateRect(hWnd, &windowRect, true);
	}

	// No status message, update progress bars
	if (patcher->statusText.empty())
	{
		if (!szStatusText.empty())
		{
			InvalidateRect(hWnd, &progressBarsRect, true);
		}

		if (patcher->state == Patcher::STATE_UPDATING_LAUNCHER)
		{
			topProgressBar->draw = true;
			bottomProgressBar->draw = false;

			// Position top progress bar as the bottom one for the duration of thetransfer
			topProgressBar->rect = bottomProgressBar->rect;
			smallStatusText = true;
			smallStatusMessage = patcher->smallStatusText;
		}
		else
		{
			topProgressBar->draw = true;
			bottomProgressBar->draw = true;

			// Reset download bar to its original position
			topProgressBar->rect.top = downloadAreaSeparatorY;
			topProgressBar->rect.bottom = downloadAreaSeparatorY + downloadBarHeight;
			topProgressBar->rect.left = 0 + playButtonPadding;
			topProgressBar->rect.right = playButton->position.x - playButtonPadding;
			smallStatusText = false;
			smallStatusMessage = L"";
		}

		szStatusText = L"";

		// Update top progress bar
		if (topProgressBar->value != patcher->fileDone || topProgressBar->maxValue != patcher->fileTotal)
		{
			// download speed math
			long long currentTime = Helper::currentTimeMillis();
			long long timeElapsed = currentTime - lastDownloadSpeedTime;
			if (timeElapsed > 500)
			{
				lastDownloadSpeedTime = currentTime;

				// this is called every 500ms
				int currentDownloadedBytes = patcher->fileDone;
				if (currentDownloadedBytes > lastDownloadedBytes)
				{
					int deltaBytes = currentDownloadedBytes - lastDownloadedBytes;
					lastDownloadSpeed = deltaBytes / (timeElapsed / 1000.0) / (1024.0 * 1024.0);
					averageDownloadSpeedList.push_back(lastDownloadSpeed);

					// remove oldest elements if the queue gets too large
					if (averageDownloadSpeedList.size() > 100)
						averageDownloadSpeedList.pop_front();
				}
				else
				{
					//lastDownloadSpeed = 0.0;
				}

				// finally
				lastDownloadedBytes = currentDownloadedBytes;
			}

			std::wstring fileName = Helper::getFileNameFromPath(patcher->currentFileName);
			int fileNameLength = fileName.size();
			if (fileNameLength > 25)
			{
				// file name too long
				fileName = L"..." + fileName.substr(fileNameLength - 25, fileNameLength);
			}

			std::wostringstream ss;
			if (patcher->state == Patcher::STATE_UPDATING_LAUNCHER)
			{
				ss << L"Downloading: " << Constants::GAME_LAUNCHER_FILE_NAME;
				topProgressBar->headerText = ss.str();
			}
			else
			{
				if (patcher->state == Patcher::STATE_CALCULATING_LOCAL_CHECKSUMS)
				{
					ss << L"Checking: " << fileName;
					topProgressBar->headerText = ss.str();
				}
				else if (patcher->state == Patcher::STATE_DOWNLOADING_REMOTE_FILES)
				{
					ss << L"Updating: " << fileName;
					topProgressBar->headerText = ss.str();
				}
			}

			char temp[256];
			snprintf(temp, sizeof(temp) - 1, " (%.2f MB/s)", lastDownloadSpeed);
			std::string szDownloadSpeed = temp;
			topProgressBar->headerText.append(Helper::s2ws(szDownloadSpeed));

			topProgressBar->setValue(patcher->fileDone);
			topProgressBar->maxValue = patcher->fileTotal;
			topProgressBar->invalidateProgressBar(hWnd);
		}

		// Update bottom progress bar
		if (bottomProgressBar->value != patcher->filesDone || bottomProgressBar->maxValue != patcher->filesTotal ||
			bottomProgressBar->value != patcher->totalBytesDone || bottomProgressBar->maxValue != patcher->totalBytesTotal)
		{
			int megabytesDownloaded = (patcher->totalBytesDone / (1024.0 * 1024.0));
			int megabytesToDownload = (patcher->totalBytesTotal / (1024.0 * 1024.0));

			double averageDownloadSpeed = 0.5f;
			for (double speed : averageDownloadSpeedList)
			{
				averageDownloadSpeed += speed;
			}
			averageDownloadSpeed /= averageDownloadSpeedList.size();

			int secondsRemaining = (megabytesToDownload - megabytesDownloaded) / averageDownloadSpeed;

			std::wostringstream ss;
			ss << L"File " << ((patcher->filesTotal == patcher->filesDone) ? patcher->filesDone : patcher->filesDone + 1) << L" of " << patcher->filesTotal;
			ss << L" (" << megabytesDownloaded << L" of " << megabytesToDownload << L" MB";
			ss << L", " << secondsRemaining / 60 << L" minutes remaining)";
			bottomProgressBar->headerText = ss.str();

			bottomProgressBar->setValue(patcher->totalBytesDone);
			bottomProgressBar->maxValue = patcher->totalBytesTotal;
			bottomProgressBar->invalidateProgressBar(hWnd);
		}
	}
	else if (patcher->statusText != szStatusText)
	{
		// patcher's status text is not empty and has been updated
		topProgressBar->draw = false;
		bottomProgressBar->draw = false; // show status text instead of progress bars

		szStatusText = patcher->statusText;
		InvalidateRect(hWnd, &progressBarsRect, true);
	}
}

void Main::paint(HDC hdc)
{
	// Create necessary objects
	Graphics g(hdc);
	RectF layout;

	// Background image
	gCache->g->DrawCachedBitmap(gCache->backgroundCachedBitmap.get(), 0, 0);
	gCache->g->FillRectangle(&gCache->fadeBrush, Rect(0, 0, windowWidth, menuBarHeight + 1));

	// Buttons
	closeButton->draw(gCache->g);
	optionsButton->draw(gCache->g);
	minimizeButton->draw(gCache->g);
	playButton->draw(gCache->g);

	// News
	for (int i = 0; i < newsLoaded; i++)
		newsLabels[i]->draw(gCache->g, &gCache->boldFontLarge, &gCache->fontSmall, &gCache->fontSmall, &gCache->grayBrush, &gCache->lightBrush, &gCache->darkGrayBrush);
	
	// Options present?
	if (options)
	{
		// Draw the background
		gCache->g->DrawLine(&gCache->darkGrayPen, Point(repairButton->position.x + menuButtonHeight, repairButton->position.y), Point(repairButton->position.x + menuButtonHeight, optionsPolygon[4].Y));
		gCache->g->FillPolygon(&gCache->darkFadeBrush, &optionsPolygon[0], optionsPolygon.size());

		// Repair button
		repairButton->draw(gCache->g);
		RectF repairRect;
		repairRect.X = repairButton->position.x + menuButtonHeight + 0.08 * menuButtonHeight;
		repairRect.Y = repairButton->position.y;
		repairRect.Width = optionsPolygon[2].X - repairRect.X;
		repairRect.Height = menuButtonHeight;
		std::wstring repairText = (repairButton->disabled && patcher->ignoreLocalVersionFile) ? L"Repairing..." : L"Repair installation";
		gCache->g->DrawString(repairText.c_str(), -1, &gCache->font, repairRect, &gCache->verticalStringFormat, &gCache->darkGrayBrush);

		// About button
		aboutButton->draw(gCache->g);
		RectF aboutRect;
		aboutRect.X = aboutButton->position.x + menuButtonHeight + 0.08 * menuButtonHeight;
		aboutRect.Y = aboutButton->position.y;
		aboutRect.Width = optionsPolygon[2].X - aboutRect.X;
		aboutRect.Height = menuButtonHeight;
		gCache->g->DrawString(L"About", -1, &gCache->font, aboutRect, &gCache->verticalStringFormat, &gCache->darkGrayBrush);
	}

	// Progress bars
	topProgressBar->drawBar(gCache->g, &gCache->darkGrayPen, &gCache ->darkFadeBrush);
	topProgressBar->drawText(gCache->g, &gCache->font, &gCache->lightBrush);
	bottomProgressBar->drawBar(gCache->g, &gCache->darkPen, &gCache->darkFadeBrush);
	bottomProgressBar->drawText(gCache->g, &gCache->font, &gCache->lightBrush);
	
	// Main status text if necessary
	if (!szStatusText.empty())
	{
		RectF statusRect = RectF(topProgressBar->rect.left, progressBarsRect.top, topProgressBar->rect.right - topProgressBar->rect.left, progressBarsRect.bottom - progressBarsRect.top);
		gCache->g->FillRectangle(&gCache->darkFadeBrush, statusRect);
		gCache->g->DrawString(szStatusText.c_str(), -1, &gCache->font, statusRect, &gCache->centeredStringFormat, &gCache->lightBrush);
	}
	else if (smallStatusText)
	{
		// Small status text to be shown during launcher update
		RECT rect;
		rect.top = downloadAreaSeparatorY;
		rect.bottom = downloadAreaSeparatorY + downloadBarHeight;
		rect.left = 0 + playButtonPadding;
		rect.right = playButton->position.x - playButtonPadding;

		int padding = (rect.right - rect.left) * 0.02;
		RectF topBarRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		RectF topBarTextRect(rect.left + padding, rect.top, rect.right - rect.left - 2 * padding, rect.bottom - rect.top);

		gCache->g->FillRectangle(&gCache->darkFadeBrush, topBarRect);
		gCache->g->DrawString(smallStatusMessage.c_str(), -1, &gCache->font, topBarTextRect, &gCache->verticalStringFormat, &gCache->lightBrush);
	}
	
	// Window title
	gCache->g->DrawString(szWindowTitle, -1, &gCache->font, PointF(3, -1), &gCache->lightBrush);

	// Overlay?
	if (overlay)
	{
		RectF overlayRect = RectF(windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
		RectF textRect = RectF(overlayRect.X + menuBarHeight, overlayRect.Y + menuBarHeight, overlayRect.Width - 2*menuBarHeight, overlayRect.Height - 3 * menuBarHeight);
		
		if (patcherOverlay)
		{
			gCache->g->FillRectangle(&gCache->veryDarkFadeBrush, overlayRect);
			gCache->g->DrawString(patcher->pauseMessage.c_str(), -1, &gCache->font, textRect, &gCache->centeredStringFormat, &gCache->lightBrush);
			
			// Continue anyways button
			RectF closeAboutButtonTextRect(0, closeAboutButton->position.y, closeAboutButton->position.x, closeAboutButton->position.height);
			gCache->g->DrawString(L"Continue anyways", -1, &gCache->font, closeAboutButtonTextRect, &gCache->centeredRightStringFormat, &gCache->darkGrayBrush);
			continueAnywaysButton->draw(gCache->g);

			// Try again button
			RectF tryAgainButtonTextRect(menuButtonHeight + menuButtonHeight * 0.8, tryAgainButton->position.y, 0.5 * windowWidth, tryAgainButton->position.height);
			std::wstring tryAgainText = L"Try again";
			gCache->g->DrawString(tryAgainText.c_str(), -1, &gCache->font, tryAgainButtonTextRect, &gCache->verticalStringFormat, &gCache->darkGrayBrush);
			tryAgainButton->draw(gCache->g);

			if (!isInAdminMode)
			{
				// Try again as admin button
				RectF tryAgainAsAdminButtonTextRect(menuButtonHeight + menuButtonHeight * 0.8, tryAgainAsAdminButton->position.y, 0.5 * windowWidth, tryAgainAsAdminButton->position.height);
				std::wstring tryAgainText = L"Try again (launch in administrator mode)";
				gCache->g->DrawString(tryAgainText.c_str(), -1, &gCache->font, tryAgainAsAdminButtonTextRect, &gCache->verticalStringFormat, &gCache->darkGrayBrush);
				tryAgainAsAdminButton->draw(gCache->g);
			}
		}
		else // about overlay
		{
			gCache->g->DrawRectangle(&gCache->lightPen, textRect);
			gCache->g->FillRectangle(&gCache->veryDarkFadeBrush, overlayRect);

			std::wstring stringToDisplay = L"";

			std::wstringstream text(Constants::ABOUT_TEXT);
			std::wstring line;
			int i = 0;
			while (std::getline(text, line))
			{
				i++;
				if (i <= currentAboutPosition)
					continue;

				if (i - currentAboutPosition < 30)
					stringToDisplay += line + L"\n";
			}
			aboutTextLines = i;

			gCache->g->DrawString(stringToDisplay.c_str(), -1, &gCache->fontSmall, textRect, nullptr, &gCache->lightBrush);
			gCache->g->FillRectangle(&gCache->grayBrush, getAboutScrollBarButtonRect());
			
			RectF closeAboutButtonTextRect(0, closeAboutButton->position.y, closeAboutButton->position.x, closeAboutButton->position.height);
			gCache->g->DrawString(L"Continue", -1, &gCache->font, closeAboutButtonTextRect, &gCache->centeredRightStringFormat, &gCache->darkGrayBrush);
			closeAboutButton->draw(gCache->g);
		}
	}

	// Copy memoryBitmap onto screen
	g.DrawImage(&gCache->memoryBitmap, 0, 0, windowWidth, windowHeight);
}

void Main::retryPatching()
{
	checkForPatch(true);
}

bool Main::restartInAdministratorMode()
{
	// Launch itself as admin
	std::wstring myPath = Helper::getExecutablePath();
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpVerb = L"runas";
	sei.lpFile = myPath.c_str();
	sei.lpParameters = L"-ignoreinstances";
	sei.hwnd = NULL;
	sei.nShow = SW_SHOWDEFAULT;

	if (!ShellExecuteEx(&sei))
	{
		return false;
	}
	else
	{
		running = false;
		return true;
	}
}

RectF Main::getAboutScrollBarButtonRect()
{
	int scrollAreaHeight = overlayTextRect.bottom - overlayTextRect.top;

	int scrollBarWidth = 0.01 * windowWidth;
	RectF scrollBarRect = RectF(overlayTextRect.right + scrollBarWidth / 2, overlayTextRect.top, 1, scrollAreaHeight);
	gCache->g->FillRectangle(&gCache->darkGrayBrush, scrollBarRect);

	int scrollButtonHeight = scrollAreaHeight * 0.1;
	float percentageScrolled = (float)currentAboutPosition / (float)(aboutTextLines - Constants::LINES_ON_ABOUT_SCREEN);
	float scrollButtonOffset = percentageScrolled * ((float)scrollAreaHeight - (float)scrollButtonHeight);
	int scrollButtonY = scrollBarRect.Y + (int)scrollButtonOffset;

	return RectF(overlayTextRect.right, scrollButtonY, scrollBarWidth, scrollButtonHeight);
}

void Main::checkForPatch(bool repair)
{
	// Reset some variables
	lastDownloadSpeedTime = 0;
	lastDownloadSpeed = 0.0;
	lastDownloadedBytes = 0;

	// In repair mode?
	repairButton->disabled = true;
	if (options) repairButton->invalidateButton(hWnd);
	playButton->disabled = true;
	playButton->invalidateButton(hWnd);
	patcher->ignoreLocalVersionFile = repair;

	// And start the patching process in another thread
	if (patcherThread.joinable()) patcherThread.join(); // this shouldn't block
	patcherThread = std::thread(&Patcher::run, patcher);
}

void Main::parseNews()
{
	// read string line by line
	std::wstringstream f(patcher->newsFile);
	std::wstring line;
	int labelIndex = 0;
	int labelRow = 0;
	std::wstring title, content, date, url;
	while (std::getline(f, line) && labelIndex < 3) {
		if (line.empty() || line == L"\r")
			continue;

		if (labelRow == 0)
			title = line;
		else if (labelRow == 1)
			content = line;
		else if (labelRow == 2)
			date = line;
		else if (labelRow == 3)
			url = line;

		labelRow++;
		if (labelRow == 4)
		{
			// finished one news article
			newsLabels[labelIndex]->setText(title, content, date, url);
			newsLabels[labelIndex]->disabled = false;
			labelRow = 0;
			labelIndex++;
		}
	}

	newsLoaded = labelIndex;
	InvalidateRect(hWnd, &patchNotesRect, true);
}