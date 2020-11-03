#pragma once
#include <vector>
#include <map>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include "Figure.hpp"

struct BoardImageInfo {
	int width;
	int height;
	int topOffset;
	int leftOffset;
	int bottomOffset;
	int rightOffest;
	int cellWidth;
	int cellHeight;
};

struct BoardInfo {
	float boardSizeMult;
	int xPos;
	int yPos;
};

struct FigureImageInfo {
	int width;
	int height;
};

class Board {
public:
	//actual board
	Figure* figures[8][8];
	Figure* selectedFigure;

	//figure sprites data
	Bitmap* figureSprites;
	std::map<int, Rect> figuresBoundingRect;
	FigureImageInfo figureInfo;

	//board sprite data
	BoardImageInfo boardImageInfo;
	BoardInfo boardInfo;
	Image* sprite;

	bool TrySelectFigure(int x, int y);
	bool TrySetFigureInCell(int x, int y);
	void SetUpFigures();

private:
	std::vector<int> selectedCell;

	std::vector<int> SelectCell(int x, int y);
};