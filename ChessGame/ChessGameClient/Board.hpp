#pragma once
#include <gdiplus.h>

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

class Board {
public:
	BoardImageInfo boardImageInfo;
	BoardInfo boardInfo;
	Image* sprite;
};