#include "Engine.hpp"

std::vector<int> selectBestMove(std::vector<std::vector<Figure*>>* map, int player, int depth, int maxDepth)
{
	sPoint bestFigureToMove; bestFigureToMove.X = 0; bestFigureToMove.Y = 0;
	sPoint bestMove; bestMove.X = 0; bestMove.Y = 0;
	int bestMoveScore = -INT32_MAX;

	for (int xPos = 0; xPos < 8; xPos++) {
		for (int yPos = 0; yPos < 8; yPos++) {

			//foreach figure find moves
			if ((*map)[xPos][yPos] && (*map)[xPos][yPos]->side == player) {
				std::vector<std::vector<int>> moves = getPossibleMoves(map, xPos, yPos, player);
				for (int xDst = 0; xDst < 8; xDst++) {
					for (int yDst = 0; yDst < 8; yDst++) {
						int moveScore = moves[xDst][yDst];
						if (moveScore != -INT32_MAX) {

							int depthScore = 0;
							if (depth < maxDepth) {
								//find best enemy move
								std::vector<std::vector<Figure*>> newMap = *map;
								simulateMove(&newMap, xPos, yPos, xDst, yDst);
								std::vector<int> moveRes = selectBestMove(&newMap, !player, depth + 1, maxDepth);
								depthScore = moveRes[4];
							}
							
							//select best move
							if (moveScore - depthScore > bestMoveScore) {
								bestMoveScore = moveScore - depthScore;
								bestMove.X = xDst;
								bestMove.Y = yDst;
								bestFigureToMove.X = xPos;
								bestFigureToMove.Y = yPos;
							}
						}
					}
				}
			}
		}
	}
	return std::vector<int>{bestFigureToMove.X, bestFigureToMove.Y, bestMove.X, bestMove.Y, bestMoveScore};
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Move generation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::vector<int>> getPossibleMoves(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, int player)
{
	Figure* figure = (*map)[xPos][yPos];

	std::vector<std::vector<int>> result = { {-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX} };

	int X, Y;
	switch (figure->type) {
	case FigureType::Pawn:

		for (int i = 1; i <= 1 + !figure->movedOnce; i++) {
			X = xPos; Y = yPos + (figure->side ? 1 : -1) * i;
			if (validateCoords(X, Y) && ((*map)[X][Y] == nullptr))
				result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
			else
				break;
		}

		for (int i = -1; i <= 1; i += 2) {
			X = xPos + i; Y = yPos + (figure->side ? 1 : -1);
			if (validateCoords(X, Y)) {
				if (((*map)[X][Y] != nullptr && (*map)[X][Y]->side != figure->side)) {
					result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
				}
			}

		}
		break;
	case FigureType::Bishop:
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				setAllCellsOnDirection(map, &result, xPos, yPos, i, j, INT32_MAX);
			}
		}
		break;
	case FigureType::Knight:
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				Y = yPos - 2 * i; X = xPos - 1 * j;
				if (validateCoords(X, Y) && ((*map)[X][Y] == nullptr || (*map)[X][Y]->side != figure->side))
					result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);

				Y = yPos - 1 * i; X = xPos - 2 * j;
				if (validateCoords(X, Y) && ((*map)[X][Y] == nullptr || (*map)[X][Y]->side != figure->side))
					result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
			}
		}
		break;
	case FigureType::Rook:
		for (int i = -1; i <= 1; i += 2) {
			setAllCellsOnDirection(map, &result, xPos, yPos, 0, i, INT32_MAX);
			setAllCellsOnDirection(map, &result, xPos, yPos, i, 0, INT32_MAX);
		}
		break;
	case FigureType::Queen:
		for (int i = -1; i <= 1; i += 1) {
			for (int j = -1; j <= 1; j += 1) {
				if (i == 0 && j == 0)
					continue;
				setAllCellsOnDirection(map, &result, xPos, yPos, i, j, INT32_MAX);
			}
		}
		break;
	case FigureType::King:
		for (int i = -1; i <= 1; i += 1) {
			for (int j = -1; j <= 1; j += 1) {
				if (i == 0 && j == 0)
					continue;
				setAllCellsOnDirection(map, &result, xPos, yPos, i, j, 1);
			}
		}
		break;
	}

	return result;
}

void setAllCellsOnDirection(std::vector<std::vector<Figure*>>* map, std::vector<std::vector<int>>* cells, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int maxMoves)
{
	Figure* figure = (*map)[xPos][yPos];
	for (int i = 1, X = xPos + xDest, Y = yPos + yDest; i <= maxMoves && validateCoords(X, Y); i++, X += xDest, Y += yDest) {
		Figure* dstFigure = (*map)[X][Y];

		if (dstFigure != nullptr) {
			if (dstFigure->side != figure->side) {
				(*cells)[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, figure->side);
			}
			break;
		}
		else {
			(*cells)[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, figure->side);
		}
	}
}

bool validateCoords(size_t xPos, size_t yPos)
{
	return (xPos < 8 && yPos < 8 && xPos >= 0 && yPos >= 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Position evaluation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int evaluateCurrentMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int player)
{
	Figure* destFigure = (*map)[xDest][yDest];
	if (destFigure) {
		switch (destFigure->type)
		{
		case FigureType::Pawn:
			return 10;
		case FigureType::Bishop:
			return 30;
		case FigureType::Knight:
			return 30;
		case FigureType::Rook:
			return 50;
		case FigureType::Queen:
			return 90;
		case FigureType::King:
			return 900;
		}
	}
	return 0;
}

void simulateMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest)
{
	(*map)[xDest][yDest] = (*map)[xPos][yPos];
	(*map)[xPos][yPos] = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Utils
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sPoint getMaxValue(std::vector<std::vector<int>>* values)
{
	sPoint pos; pos.X = 0; pos.Y = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((*values)[i][j] > (*values)[pos.X][pos.Y]) {
				pos.X = i;
				pos.Y = j;
			}
		}
	}
	return pos;
}

/*	std::vector<std::vector<Figure*>> new_map = {{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr}, };*/