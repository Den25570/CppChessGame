#pragma once

#include <string>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include "Board.hpp"

//Class to manage all draw functionality

struct Button {
	int id;
	Rect rect;
	std::wstring text;
	bool isShown;
};

class WindowPainter {
public:
	RECT windowRect;
	int xMousePos;
	int yMousePos;

	HDC bufferDC;
	HDC memoryDC;

	std::vector<Button> buttons;
	void CreateButton(Rect rect, std::wstring text, bool isShown, int id);
	void ChangeButtonVisibility(int id);
	void DrawButtons();

	//Field
	void DrawField(Board* board);
	void DrawFigures(Board* board);
	void DrawSelectedFigure(Board* board);
	void DrawHintMoves(Board* board);
	void DrawCurrentMoveCell(Board* board);
	void DrawDangerHints(Board* board, int currentSide);
	void LoadSprites(Board* board);

	//Logger
	void DrawLoggerWindow(Rect rect);

	void CreateBuffer(HWND hwnd);
	void SetHDC(HDC hdc);
	void SetWindow(HWND hwnd, Board* board, INT bottomMargin);

private:
	HDC currentHDC;	
	Graphics* currentGraphics;

	void LoadBoardSprite(Board* board, std::wstring path);
	void LoadFigureSprites(Board* board, std::wstring folderPath);
};
