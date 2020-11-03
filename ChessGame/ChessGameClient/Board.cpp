#include "Board.hpp"

bool Board::TrySelectFigure(int x, int y)
{
	selectedCell = SelectCell(x, y);
	this->selectedFigure = this->figures[selectedCell[0]][selectedCell[1]];
	return this->selectedFigure != nullptr;
}

bool Board::TrySetFigureInCell(int x, int y)
{
	std::vector<int> destCell = SelectCell(x, y);
	figures[selectedCell[0]][selectedCell[1]] = nullptr;
	figures[destCell[0]][destCell[1]] = this->selectedFigure;
	this->selectedFigure = nullptr;
	return true;
}

void Board::SetUpFigures() {
	Figure* figures[8][8] = {
		new Figure(Rook, 1),new Figure(Knight, 1),new Figure(Bishop, 1),new Figure(Queen, 1),new Figure(King, 1),new Figure(Bishop, 1),new Figure(Knight, 1),new Figure(Rook, 1),
		new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		new Figure(Pawn, 0) ,new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),
		new Figure(Rook, 0) ,new Figure(Knight, 0),new Figure(Bishop, 0),new Figure(Queen, 0),new Figure(King, 0),new Figure(Bishop, 0),new Figure(Knight, 0),new Figure(Rook, 0)
	};

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->figures[i][j] = figures[j][i];
		}
	}
}

std::vector<int> Board::SelectCell(int x, int y)
{
	int startX = this->boardInfo.xPos + this->boardImageInfo.leftOffset * this->boardInfo.boardSizeMult;
	int startY = this->boardInfo.yPos + this->boardImageInfo.topOffset * this->boardInfo.boardSizeMult;
	int boardX = x - startX;
	int boardY = y - startY;
	int i = boardX / (this->boardImageInfo.cellWidth * this->boardInfo.boardSizeMult);
	int j = boardY / (this->boardImageInfo.cellHeight * this->boardInfo.boardSizeMult);
	return std::vector<int> { i, j };
}