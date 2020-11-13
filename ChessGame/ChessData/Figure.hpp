#pragma once

enum FigureType {
	King,
	Queen,
	Bishop,
	Knight,
	Rook,
	Pawn,
};

enum Side {
	White = 0,
	Black = 1
};

class Figure {
public:
	FigureType type;
	Side side;
	bool movedOnce;
	bool possibleMovesMap[8][8];

	Figure(FigureType _type, int _side) {
		this->type = _type;
		this->side = (Side)_side;
		this->movedOnce = false;
	}

	Figure(FigureType _type, Side _side) {
		this->type = _type;
		this->side = _side;
		this->movedOnce = false;
	}
};