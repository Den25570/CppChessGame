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

struct Panel {
	int id;
	Rect rect;
	std::vector<std::wstring> texts;
	std::vector<Color> textsColor;
	bool isShown;
	bool dark;
};

class WindowPainter {
public:
	//window info
	RECT windowRect;
	int xMousePos;
	int yMousePos;
	int clickedXMousePos;
	int clickedYMousePos;

	//Memory buffers
	HDC bufferDC;
	HDC memoryDC;

	//board
	Board* board;

	//controls
	std::vector<Panel> panels;
	std::vector<Button> buttons;
	Rect* intersectedRect;
	Rect deadWhiteFiguresRect;
	Rect deadBlackFiguresRect;

	Button* CreateButton(Rect rect, std::wstring text, bool isShown, int id);
	Panel* CreatePanel(Rect rect, std::vector<std::wstring> texts, std::vector<Color> textsColor, bool isShown, int id, bool dark);
	void ChangeButtonVisibility(int id);
	void ChangePanelVisibility(int id);
	void ChangeAllControlsVisibility();
	void DrawControls();
	void DrawButtons();
	void DrawPanels();
	void DrawMenu();
	int ClickButton();
	bool NeedInvalidation();

	//Field
	void DrawField();
	void DrawFigures();
	void DrawSelectedFigure();
	void DrawHintMoves();
	void DrawCurrentMoveCell();
	void DrawDeadFigures();
	void DrawDangerHints(int currentSide);
	void LoadSprites();

	//board
	void SetBoard(Board* board);

	//Logger
	void DrawLoggerWindow(Rect rect);

	void CreateBuffer(HWND hwnd);
	void SetHDC(HDC hdc);
	void SetWindow(HWND hwnd, INT bottomMargin, INT topMargin);

private:
	HDC currentHDC;	
	Graphics* currentGraphics;

	void LoadBoardSprite(std::wstring path);
	void LoadFigureSprites(std::wstring folderPath);
};
