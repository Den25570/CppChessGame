#pragma once

#include <gdiplus.h>

enum FigureType {
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King,
};

class Figure {
public:
	int xBoardPos;
	int yBoardPos;

	int xWindowPos;
	int yWindowPos;

	FigureType type;
	Image* sprite;
};