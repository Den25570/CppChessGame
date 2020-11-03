#pragma once

#include <string>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include "Board.hpp"

//Class to manage all draw functionality

class WindowPainter {
public:
	RECT windowRect;
	int xMousePos;
	int yMousePos;

	HDC bufferDC;
	HDC memoryDC;

	void DrawField(Board* board);
	void DrawFigures(Board* board);
	void DrawSelectedFigure(Board* bnoard);

	void LoadSprites(Board* board);

	void CreateBuffer(HWND hwnd);
	void SetHDC(HDC hdc);
	void SetWindow(HWND hwnd);

private:
	HDC currentHDC;	
	Graphics* currentGraphics;

	void LoadBoardSprite(Board* board, std::wstring path);
	void LoadFigureSprites(Board* board, std::wstring folderPath);
};
