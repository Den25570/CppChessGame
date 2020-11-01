#pragma once

#include <string>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

//Class to manage all draw functionality

class WindowPainter {
public:
	void DrawField();
	void DrawFigures();
	void LoadImageFromFile(std::wstring path);
	void SetHDC(HDC hdc);
	void SetWindow(HWND hwnd);

	void FreeResources();

private:
	RECT windowRect;
	HDC currentHDC;
	Graphics* currentGraphics;
	Image* fieldImage;
};