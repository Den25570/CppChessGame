#include "Board.hpp"
#include <string>
#include "Engine.hpp"

bool Board::TrySelectFigure(Point pos, int side)
{	
	this->selectedCell = selectCell(pos);

	if (!(this->selectedCell.X > 7 || this->selectedCell.X < 0 || this->selectedCell.Y > 7 || this->selectedCell.Y < 0)) {
		this->selectedFigure = this->figures[selectedCell.X][selectedCell.Y];
		if (this->selectedFigure && this->selectedFigure->side == side)
			return true;
	}
	this->selectedFigure = nullptr;
	return false;	
}

MoveType Board::TryMove(Point pos, std::vector<int>* move)
{
	MoveType moveType = MoveType::None;
	Point destCell = selectCell(pos);
	if (!(destCell.X > 7 || destCell.X < 0 || destCell.Y > 7 || destCell.Y < 0)) {
		if (this->selectedFigure && this->selectedFigure->possibleMovesMap[destCell.X][destCell.Y]) {			
			moveType = MoveType::Default;

			//Castling
			if (figures[selectedCell.X][selectedCell.Y]->type == King && figures[selectedCell.X][selectedCell.Y]->totalMoves == 0) {
				if ((destCell.X == selectedCell.X + 2)) {
					figures[selectedCell.X + 1][selectedCell.Y] = figures[selectedCell.X + 3][selectedCell.Y];
					figures[selectedCell.X + 3][selectedCell.Y] = nullptr;
					moveType = MoveType::ShortCastling;
				}
				else if (destCell.X == selectedCell.X - 2) {
					figures[selectedCell.X - 2][selectedCell.Y] = figures[selectedCell.X - 4][selectedCell.Y];
					figures[selectedCell.X - 4][selectedCell.Y] = nullptr;
					moveType = MoveType::LongCastling;
					destCell.X -= 1;
				}
			}

			//Promotion
			if (figures[selectedCell.X][selectedCell.Y]->type == Pawn && (destCell.Y == 0 || destCell.Y == 7)) {
				figures[selectedCell.X][selectedCell.Y]->type = Queen;
				moveType = MoveType::Promotion;
			}

			(*move).push_back(destCell.X);
			(*move).push_back(destCell.Y);

			//move
			figures[selectedCell.X][selectedCell.Y] = nullptr;
			if (figures[destCell.X][destCell.Y] != nullptr) {
				(*move).push_back(figures[destCell.X][destCell.Y]->type);
				deadFigures.push_back(figures[destCell.X][destCell.Y]);
			}
			figures[destCell.X][destCell.Y] = this->selectedFigure;

			this->selectedFigure->totalMoves++;
		}
	}
	this->selectedFigure = nullptr;
	return moveType;
}

void Board::InitGame() {
	std::vector<std::vector<Figure*>> figures = std::vector<std::vector<Figure*>>{
		{new Figure(Rook, 1),new Figure(Knight, 1),new Figure(Bishop, 1),new Figure(Queen, 1),new Figure(King, 1),new Figure(Bishop, 1),new Figure(Knight, 1),new Figure(Rook, 1)},
		{new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1),new Figure(Pawn, 1)},
		{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
		{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
		{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
		{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
		{new Figure(Pawn, 0) ,new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0),new Figure(Pawn, 0)},
		{new Figure(Rook, 0) ,new Figure(Knight, 0),new Figure(Bishop, 0),new Figure(Queen, 0),new Figure(King, 0),new Figure(Bishop, 0),new Figure(Knight, 0),new Figure(Rook, 0)}
	};

	deadFigures.clear();

	for (int i = 0; i < 8; i++) {
		this->figures.resize(8);
		for (int j = 0; j < 8; j++) {
			this->figures[i].resize(8);
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->figures[i][j] = figures[j][i];
		}
	}
}

void Board::SetAllPossibleMoves(int side)
{
	//process all possible moves
	isMate = true;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (this->figures[i][j] && this->figures[i][j]->side == side) {
				std::vector<std::vector<int>> moves = getPossibleMoves(&(this->figures), i, j, 0, nullptr, false);
				bool noMovesRemain = FilterUserMoves(&(this->figures), &moves, i, j, side);
				isMate &= noMovesRemain;
				for (int x= 0; x < 8; x++) {
					for (int y = 0; y < 8; y++) {
						this->figures[i][j]->possibleMovesMap[x][y] = moves[x][y] != -INT32_MAX;
					}
				}
			}
		}
	}
}

void Board::GetFiguresAttackingKing(int side)
{
	this->figuresAttackingKing.clear();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (this->figures[i][j] && this->figures[i][j]->side == !side) {
				int bestScore = -INT32_MAX;
				std::vector<std::vector<int>> moves = getPossibleMoves(&(this->figures), i, j, 0, &bestScore, true);
				if (bestScore >= beatScore[0] * 0.75) {
					{
						this->figuresAttackingKing.push_back(Point(i, j));
					}
				}
			}
		}
	}
}

MoveType Board::AIMove(int side, int difficulty, std::vector<int> *move)
{
	MoveType moveType = MoveType::None;

	(*move) = selectBestMove(threadPool, &(this->figures), side, 1, difficulty , true);
	(*move).pop_back();

	//move
	moveType = MoveType::Default;

	//Castling
	if (figures[(*move)[0]][(*move)[1]]->type == King && figures[(*move)[0]][(*move)[1]]->totalMoves == 0) {
		if (((*move)[2] == (*move)[0] + 2)) {
			figures[(*move)[0] + 1][(*move)[1]] = figures[(*move)[0] + 3][(*move)[1]];
			figures[(*move)[0] + 3][(*move)[1]] = nullptr;
			moveType = MoveType::ShortCastling;
		}
		else if ((*move)[2] == (*move)[0] - 2) {
			figures[(*move)[0] - 2][(*move)[1]] = figures[(*move)[0] - 4][(*move)[1]];
			figures[(*move)[0] - 4][(*move)[1]] = nullptr;
			moveType = MoveType::LongCastling;

			(*move)[2] -= 1;
		}
	}

	//Promotion
	if (figures[(*move)[0]][(*move)[1]]->type == Pawn && ((*move)[3] == 0 || (*move)[3] == 7)) {
		figures[(*move)[0]][(*move)[1]]->type = Queen;
		moveType = MoveType::Promotion;
	}

	if (figures[(*move)[2]][(*move)[3]] != nullptr) {
		(*move).push_back(figures[(*move)[2]][(*move)[3]]->type);
		deadFigures.push_back(figures[(*move)[2]][(*move)[3]]);
		figures[(*move)[2]][(*move)[3]] = nullptr;
	}
	figures[(*move)[2]][(*move)[3]] = figures[(*move)[0]][(*move)[1]];
	figures[(*move)[0]][(*move)[1]]->totalMoves++;
	figures[(*move)[0]][(*move)[1]] = nullptr;

	return moveType;
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
	//return Point(i > 7 ? 7: i < 0 ? 0 : i, j > 7 ? 7 : j < 0 ? 0 : j);
}