#include "Board.hpp"
#include <string>
#include "Engine.hpp"

bool Board::TrySelectFigure(Point pos, int side)
{
	this->selectedFigure = nullptr;
	this->selectedCell = selectCell(pos);

	this->selectedFigure = this->figures[selectedCell.X][selectedCell.Y];
	if (this->selectedFigure && this->selectedFigure->side == side)
		return true;
	else
		return false;
	
}

bool Board::TryMove(Point pos, std::vector<int>* move)
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

		(*move).push_back(destCell.X);
		(*move).push_back(destCell.Y);

		return true;
	}
	else {
		this->selectedFigure = nullptr;
		return false;
	}
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
	bool isMate = true;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (this->figures[i][j] && this->figures[i][j]->side == side) {
				std::vector<std::vector<int>> moves = getPossibleMoves(&(this->figures), i, j, 0, nullptr);
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
				std::vector<std::vector<int>> moves = getPossibleMoves(&(this->figures), i, j, 0, &bestScore);
				if (bestScore >= beatScore[0] * 0.75) {
					{
						this->figuresAttackingKing.push_back(Point(i, j));
					}
				}
			}
		}
	}
}

std::vector<int> Board::AIMove(int side)
{
	std::vector<int> move = selectBestMove(&(this->figures), side, 1, 4);

	//move
	if (figures[move[2]][move[3]] != nullptr) {
		delete(figures[move[2]][move[3]]);
	}
	figures[move[2]][move[3]] = figures[move[0]][move[1]];
	figures[move[0]][move[1]]->movedOnce = true;
	figures[move[0]][move[1]] = nullptr;

	return move;
}

Point Board::selectCell(Point pos)
{
	int startX = this->boardInfo.rect.X + this->boardImageInfo.leftOffset * this->boardInfo.boardSizeMult;
	int startY = this->boardInfo.rect.Y + this->boardImageInfo.topOffset * this->boardInfo.boardSizeMult;
	int boardX = pos.X - startX;
	int boardY = pos.Y - startY;
	int i = boardX / (this->boardImageInfo.cellWidth * this->boardInfo.boardSizeMult);
	int j = boardY / (this->boardImageInfo.cellHeight * this->boardInfo.boardSizeMult);

	return Point(i > 7 ? 7: i < 0 ? 0 : i, j > 7 ? 7 : j < 0 ? 0 : j);
}