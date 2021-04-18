#pragma once
#include <vector>
#include <map>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include "Figure.hpp"
#include "ThreadPool.hpp"

struct BoardImageInfo {
	int width;
	int height;
	int topOffset;
	int leftOffset;
	int bottomOffset;
	int rightOffest;
	int cellWidth;
	int cellHeight;

	Color borderColor;
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
	std::vector<Figure*> deadFigures;
	Figure* backUpFigures[8][8];
	Figure* selectedFigure;
	Point selectedCell;
	std::vector<Point> figuresAttackingKing;
	bool isMate;

	//figure sprites data
	Bitmap* figureSprites;
	std::map<int, Rect> figuresBoundingRect;
	FigureImageInfo figureInfo;

	//board sprite data
	BoardImageInfo boardImageInfo;
	BoardInfo boardInfo;
	Image* sprite;
	Color darkBoardColor;
	Color lightBoardColor;

	//utils
	ThreadPool* threadPool;

	bool TrySelectFigure(Point pos, int side);
	MoveType TryMove(Point pos, std::vector<int>* move);
	MoveType AIMove(int side, int difficulty, std::vector<int>* move);
	void InitGame();
	void SetAllPossibleMoves(int side);
	void GetFiguresAttackingKing(int side);
	Point selectCell(Point pos);
};