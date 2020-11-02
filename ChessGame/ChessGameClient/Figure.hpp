#pragma once

enum FigureType {
	King,
	Queen,
	Bishop,
	Knight, 
	Rook,
	Pawn,	
}; 

class Figure {
public:
	FigureType type;
	int side; // 0 - white, 1 - black

	Figure(FigureType _type, int _side) {
		this->type = _type;
		this->side = _side;
	}
};