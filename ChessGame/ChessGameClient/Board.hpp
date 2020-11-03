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
	Rect rect;
	RECT tagRect;
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
	bool beatbleRegionsForWhite[8][8];
	bool beatbleRegionsForBlack[8][8];

	//figure sprites data
	Bitmap* figureSprites;
	std::map<int, Rect> figuresBoundingRect;
	FigureImageInfo figureInfo;

	//board sprite data
	BoardImageInfo boardImageInfo;
	BoardInfo boardInfo;
	Image* sprite;

	bool TrySelectFigure(int x, int y);
	bool TryMove(int x, int y);
	void SetUpFigures();
	void SetAllPossibleMoves();

	RECT AIMove(int side);

private:
	std::vector<int> selectedCell;

	std::vector<int> selectCell(int x, int y);
	void setPossibleMoves(Point position);
	void setAllCellsOnDirection(Point src, Point dir, int maxMoves = INT_MAX, int beatFlag = 0);
	bool isKingBeatable(Point pos);
	bool validateCoords(int x, int y, int side);
};