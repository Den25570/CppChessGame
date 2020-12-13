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

enum MoveType {
	None,
	Default,
	ShortCastling,
	LongCastling,
	LongPawnKill,
	Promotion,
};

class Figure {
public:
	FigureType type;
	bool isOriginalyPawn;
	int promotionMoveNum;

	Side side;
	int totalMoves;
	bool possibleMovesMap[8][8];

	Figure(FigureType _type, int _side) {
		this->type = _type;
		this->side = (Side)_side;
		this->totalMoves = 0;
		isOriginalyPawn = (_type == Pawn);
	}

	Figure(FigureType _type, Side _side) {
		this->type = _type;
		this->side = _side;
		this->totalMoves = 0;
		this->isOriginalyPawn = (_type == Pawn);
	}
};