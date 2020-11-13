#include "Engine.hpp"

std::vector<Point> selectBestMove(std::vector<std::vector<Figure*>>* map, int player)
{
	Point bestFigureToMove; bestFigureToMove.x = 0; bestFigureToMove.y = 0;
	Point bestMove; bestMove.x = 0; bestMove.y = 0;
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
							//find best enemy move
							std::vector<std::vector<Figure*>> newMap = *map;
							simulateMove(&newMap, xPos, yPos, xDst, yDst);
							
							int bestEnemyMoveScore = -INT32_MAX;
							for (int xPos = 0; xPos < 8; xPos++) {
								for (int yPos = 0; yPos < 8; yPos++) {
									//foreach enemy figure find moves
									if (newMap[xPos][yPos] && newMap[xPos][yPos]->side == player) {
										std::vector<std::vector<int>> moves = getPossibleMoves(&newMap, xPos, yPos, player);
										Point bestDst = getMaxValue(&moves);
										//select best enemy move
										if (moves[bestDst.x][bestDst.y] > bestEnemyMoveScore) {
											bestEnemyMoveScore = moves[bestDst.x][bestDst.y];
										}
									}
								}
							}
							//select best move
							if (moveScore - bestEnemyMoveScore > bestMoveScore) {
								bestMoveScore = moveScore - bestEnemyMoveScore;
								bestMove.x = xDst;
								bestMove.y = yDst;
								bestFigureToMove.x = xPos;
								bestFigureToMove.y = yPos;
							}
						}
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Move generation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::vector<int>> getPossibleMoves(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, int player)
{
	Figure* figure = (*map)[xPos][yPos];

	std::vector<std::vector<int>> result = {{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX},
											{-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX,-INT32_MAX}};

	int x, y;
	switch (figure->type) {
	case FigureType::Pawn:

		for (int i = 1; i <= 1 + !figure->movedOnce; i++) {
			x = xPos; y = yPos + (figure->side ? 1 : -1) * i;
			if (validateCoords(x, y) && ((*map)[x][y] == nullptr))
				result[x][y] = evaluateCurrentMove(map, xPos, yPos, x, y, player);
			else
				break;
		}

		for (int i = -1; i <= 1; i += 2) {
			x = xPos + i; y = yPos + (figure->side ? 1 : -1);
			if (validateCoords(x, y)) {
				if (((*map)[x][y] != nullptr && (*map)[x][y]->side != figure->side)) {
					result[x][y] = evaluateCurrentMove(map, xPos, yPos, x, y, player);
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
				y = yPos - 2 * i; x = xPos - 1 * j;
				if (validateCoords(x, y) && ((*map)[x][y] == nullptr || (*map)[x][y]->side != figure->side))
					result[x][y] = evaluateCurrentMove(map, xPos, yPos, x, y, player);

				y = yPos - 1 * i; x = xPos - 2 * j;
				if (validateCoords(x, y) && ((*map)[x][y] == nullptr || (*map)[x][y]->side != figure->side))
					result[x][y] = evaluateCurrentMove(map, xPos, yPos, x, y, player);
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
	for (int i = 1, x = xPos + xDest, y = yPos + yDest; i <= maxMoves && validateCoords(x, y); i++, x += xDest, y += yDest) {
		Figure* dstFigure = (*map)[x][y];

		if (dstFigure != nullptr) {
			if (dstFigure->side != figure->side) {
				(*cells)[x][y] = evaluateCurrentMove(map, xPos, yPos, x, y, figure->side);
			}
			break;
		}
		else {
			(*cells)[x][y] = evaluateCurrentMove(map, xPos, yPos, x, y, figure->side);
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
			return (destFigure->type == player ? -1 : 1) * 10;
		case FigureType::Bishop:
			return (destFigure->type == player ? -1 : 1) * 30;
		case FigureType::Knight:
			return (destFigure->type == player ? -1 : 1) * 30;
		case FigureType::Rook:
			return (destFigure->type == player ? -1 : 1) * 50;
		case FigureType::Queen:
			return (destFigure->type == player ? -1 : 1) * 90;
		case FigureType::King:
			return (destFigure->type == player ? -1 : 1) * 900;
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

Point getMaxValue(std::vector<std::vector<int>>* values)
{
	Point pos; pos.x = 0; pos.y = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((*values)[i][j] > (*values)[pos.x][pos.y]) {
				pos.x = i;
				pos.y = j;
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