#include "Board.hpp"
#include <string>

bool Board::TrySelectFigure(Point pos, int side)
{
	this->selectedFigure = nullptr;
	selectedCell = selectCell(pos);

	this->selectedFigure = this->figures[selectedCell.X][selectedCell.Y];
	if (this->selectedFigure && this->selectedFigure->side == side)
		return true;
	else
		return false;
	
}

bool Board::TryMove(Point pos)
{
	Point destCell = selectCell(pos);
	if (this->selectedFigure->possibleMovesMap[destCell.X][destCell.Y]) {
		//move
		figures[selectedCell.X][selectedCell.Y] = nullptr;
		if (figures[destCell.X][destCell.Y] != nullptr) {
			delete(figures[destCell.X][destCell.Y]);
		}
		figures[destCell.X][destCell.Y] = this->selectedFigure;

		this->selectedFigure->movedOnce = true;

		this->selectedFigure = nullptr;

		//update maps
		this->SetAllPossibleMoves();

		return true;
	}
	else {
		this->selectedFigure = nullptr;
		return false;
	}
}

void Board::InitGame() {
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

	this->SetAllPossibleMoves();
}

std::vector<std::vector<int>> Board::getPossibleMoves(Point position)
{
	std::vector<std::vector<int>> result = { {false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false}, 
	{false,false,false,false,false,false,false,false}, 
	{false,false,false,false,false,false,false,false}, 
	{false,false,false,false,false,false,false,false}, 
	{false,false,false,false,false,false,false,false}, 
	{false,false,false,false,false,false,false,false}, 
	{false,false,false,false,false,false,false,false}, };

	Figure* figure = this->figures[position.X][position.Y];

	int x, y;
	switch (figure->type) {
	case FigureType::Pawn:

		for (int i = 1; i <= 1 + !figure->movedOnce; i++) {
			x = position.X; y = position.Y + (figure->side ? 1 : -1) * i;
			if (validateCoords(Point(x, y), figure->side) && (this->figures[x][y] == nullptr))
				result[x][y] = true;
			else
				break;
		}

		for (int i = -1; i <= 1; i += 2) {
			x = position.X + i; y = position.Y + (figure->side ? 1 : -1);
			if (validateCoords(Point(x, y), figure->side)) {
				if ((this->figures[x][y] != nullptr && this->figures[x][y]->side != figure->side)) {
					result[x][y] = true;
				}
			}

		}
		break;
	case FigureType::Bishop:
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				this->setAllCellsOnDirection(&result, position, Point(i, j));
			}
		}
		break;
	case FigureType::Knight:
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j+=2) {
				y = position.Y - 2 * i; x = position.X - 1 * j;
				if (validateCoords(Point(x, y), figure->side) && (this->figures[x][y] == nullptr || this->figures[x][y]->side != figure->side))
					result[x][y] = true;

				y = position.Y - 1 * i; x = position.X - 2 * j;
				if (validateCoords(Point(x, y), figure->side) && (this->figures[x][y] == nullptr || this->figures[x][y]->side != figure->side))
					result[x][y] = true;
			}
		}
		break;
	case FigureType::Rook:
		for (int i = -1; i <= 1; i += 2) {
			this->setAllCellsOnDirection(&result, position, Point(0, i));
			this->setAllCellsOnDirection(&result, position, Point(i, 0));
		}
		break;
	case FigureType::Queen:
		for (int i = -1; i <= 1; i += 1) {
			for (int j = -1; j <= 1; j += 1) {
				if (i == 0 && j == 0)
					continue;
				this->setAllCellsOnDirection(&result, position, Point(i, j));
			}
		}
		break;
	case FigureType::King:
		for (int i = -1; i <= 1; i += 1) {
			for (int j = -1; j <= 1; j += 1) {
				if (i == 0 && j == 0)
					continue;
				this->setAllCellsOnDirection(&result, position, Point(i, j), 1);
			}
		}
		break;
	}

	return result;
}

void Board::SetAllPossibleMoves()
{
	//process all possible moves
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (this->figures[i][j] != nullptr) {
				std::vector<std::vector<int>> moves = getPossibleMoves(Point(i, j));
				for (int x= 0; x < 8; x++) {
					for (int y = 0; y < 8; y++) {
						this->figures[i][j]->possibleMovesMap[x][y] = moves[x][y];
					}
				}
			}
		}
	}
//	this->filterAllMoves();
}

RECT Board::AIMove(int side)
{
	//
	std::string moveFromEngine = "e2e4";
	//

	/*Select Random*/
	/*for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ()
		}
	}*/

	return RECT();
}

Point Board::selectCell(Point pos)
{
	int startX = this->boardInfo.rect.X + this->boardImageInfo.leftOffset * this->boardInfo.boardSizeMult;
	int startY = this->boardInfo.rect.Y + this->boardImageInfo.topOffset * this->boardInfo.boardSizeMult;
	int boardX = pos.X - startX;
	int boardY = pos.Y - startY;
	int i = boardX / (this->boardImageInfo.cellWidth * this->boardInfo.boardSizeMult);
	int j = boardY / (this->boardImageInfo.cellHeight * this->boardInfo.boardSizeMult);
	return Point(i, j);
}

void Board::setAllCellsOnDirection(std::vector<std::vector<int>>* cells, Point src, Point dir, int maxMoves)
{
	Figure* figure = this->figures[src.X][src.Y];
	for (int i = 1, x = src.X + dir.X, y = src.Y + dir.Y; i <= maxMoves && validateCoords(Point(x, y), figure->side); i++, x += dir.X, y += dir.Y) {
		Figure* dstFigure = this->figures[x][y];

		if (dstFigure != nullptr) {
			if (dstFigure->side != figure->side) {
				(*cells)[x][y] = true;
			}
			break;
		}
		else {
			(*cells)[x][y] = true;
		}
	}
}

bool Board::isKingBeatable(Point pos)
{
	return true;
}

bool Board::validateCoords(Point pos, int side)
{
	return (pos.X < 8 && pos.Y < 8 && pos.X >= 0 && pos.Y >= 0);
}

void Board::filterAllMoves()
{
	Point whiteKingPos;
	Point blackKingPos;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->backUpFigures[i][j] = this->figures[i][j];
			if (this->figures[i][j]->type == FigureType::King && this->figures[i][j]->side == 0)
				whiteKingPos = Point(i, j);
			else if (this->figures[i][j]->type == FigureType::King && this->figures[i][j]->side == 1)
				blackKingPos = Point(i, j);
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (this->figures[i][j] != nullptr) {
				std::vector<std::vector<int>> moves = getPossibleMoves(Point(i, j));
				if (this->figures[i][j]->type == FigureType::King && this->figures[i][j]->side == 0) {

				}
				else if (this->figures[i][j]->type == FigureType::King && this->figures[i][j]->side == 1) {

				}
				else {

				}
			}
		}
	}
	
}
