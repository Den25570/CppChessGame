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
	Figure* backUpFigures[8][8];
	Figure* selectedFigure;

	//figure sprites data
	Bitmap* figureSprites;
	std::map<int, Rect> figuresBoundingRect;
	FigureImageInfo figureInfo;

	//board sprite data
	BoardImageInfo boardImageInfo;
	BoardInfo boardInfo;
	Image* sprite;

	bool TrySelectFigure(Point pos, int side);
	bool TryMove(Point pos);
	void InitGame();
	void SetAllPossibleMoves();

	RECT AIMove(int side);

private:
	Point selectedCell;

	Point selectCell(Point pos);
	std::vector<std::vector<int>> getPossibleMoves(Point position);
	void setAllCellsOnDirection(std::vector<std::vector<int>>* cells, Point src, Point dir, int maxMoves = INT_MAX);
	bool isKingBeatable(Point pos);
	bool validateCoords(Point, int side);
	void filterAllMoves();
};