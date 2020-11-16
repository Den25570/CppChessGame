#pragma once
#include <vector>
#include <map>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include "Figure.hpp"
#include "Engine.hpp"

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
	std::vector<std::vector<Figure*>> figures;
	Figure* backUpFigures[8][8];
	Figure* selectedFigure;
	std::vector<Point> figuresAttackingKing;

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
	void SetAllPossibleMoves(int side);
	void GetFiguresAttackingKing(int side);

	void AIMove(int side);

private:
	Point selectedCell;

	Point selectCell(Point pos);
};