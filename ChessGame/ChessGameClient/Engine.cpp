#include "Engine.hpp"
#include <time.h>

std::vector<int> selectBestMove(ThreadPool* threadPool, std::vector<std::vector<Figure*>>* map, int player, int depth, int maxDepth, bool useFilter)
{
	srand(static_cast<unsigned int>(time(0)));
	sPoint bestFigureToMove; bestFigureToMove.X = 0; bestFigureToMove.Y = 0;
	sPoint bestMove; bestMove.X = 0; bestMove.Y = 0;
	int bestMoveScore = -INT32_MAX;

	for (int xPos = 0; xPos < 8; xPos++) {
		for (int yPos = 0; yPos < 8; yPos++) {
			//foreach figure find moves
			if ((*map)[xPos][yPos] && (*map)[xPos][yPos]->side == player) {
				int bestScore = -INT32_MAX;

				std::vector<std::vector<int>> moves = getPossibleMoves(map, xPos, yPos, player, &bestScore, false);
				if (useFilter && (depth <= maxDepth ))
					FilterUserMoves(map, &moves, xPos, yPos, player);

				for (int xDst = 0; xDst < 8; xDst++) {
					for (int yDst = 0; yDst < 8; yDst++) {						
						if (moves[xDst][yDst] != -INT32_MAX) {

							//return if can beat king
							if (moves[xDst][yDst] == beatScore[0]) {
								return std::vector<int>{xPos, yPos, xDst, yDst, moves[xDst][yDst]};
							}

							moves[xDst][yDst] += (rand() % randomFactor * 2) - randomFactor;
							int moveScore = moves[xDst][yDst];
							int depthScore = 0;
							if (depth < (moves[xDst][yDst] < bestScore ? maxDepth - 1 : maxDepth)) {
								//find best enemy move
								std::vector<std::vector<Figure*>> newMap = *map;
								simulateMove(&newMap, xPos, yPos, xDst, yDst);
								std::vector<int> moveRes = selectBestMove(threadPool, &newMap, !player, depth + 1, moveScore < bestScore ? maxDepth - 1 : maxDepth, useFilter);
								unSimulateMove(map, xPos, yPos, xDst, yDst);

								//continue if enemy can beat king
								if (moveRes[4] == beatScore[0]) {
									moves[xDst][yDst] = -INT32_MAX;
									continue;
								}
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

std::vector<std::vector<int>> getPossibleMoves(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, int player, int* bestScoreOut, bool ignoreCastling)
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

		for (int i = 1; i <= 1 + !(figure->totalMoves > 0); i++) {
			X = xPos; Y = yPos + (figure->side ? 1 : -1) * i;
			if (validateCoords(X, Y) && ((*map)[X][Y] == nullptr)) {
				result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
				if (bestScoreOut && (*bestScoreOut) < result[X][Y])
					(*bestScoreOut) = result[X][Y];
			}						
			else
				break;
		}

		for (int i = -1; i <= 1; i += 2) {
			X = xPos + i; Y = yPos + (figure->side ? 1 : -1);
			if (validateCoords(X, Y)) {
				if (((*map)[X][Y] != nullptr && (*map)[X][Y]->side != figure->side)) {
					result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
					if (bestScoreOut && (*bestScoreOut) < result[X][Y])
						(*bestScoreOut) = result[X][Y];
				}
			}
		}
		break;
	case FigureType::Bishop:
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				setAllCellsOnDirection(map, &result, xPos, yPos, i, j, INT32_MAX, bestScoreOut);
			}
		}
		break;
	case FigureType::Knight:
		for (int i = -1; i <= 1; i += 2) {
			for (int j = -1; j <= 1; j += 2) {
				Y = yPos - 2 * i; X = xPos - 1 * j;
				if (validateCoords(X, Y) && ((*map)[X][Y] == nullptr || (*map)[X][Y]->side != figure->side)) {
					result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
					if (bestScoreOut && (*bestScoreOut) < result[X][Y])
						(*bestScoreOut) = result[X][Y];
				}
					

				Y = yPos - 1 * i; X = xPos - 2 * j;
				if (validateCoords(X, Y) && ((*map)[X][Y] == nullptr || (*map)[X][Y]->side != figure->side)) {
					result[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, player);
					if (bestScoreOut && (*bestScoreOut) < result[X][Y])
						(*bestScoreOut) = result[X][Y];
				}
			}
		}
		break;
	case FigureType::Rook:
		for (int i = -1; i <= 1; i += 2) {
			setAllCellsOnDirection(map, &result, xPos, yPos, 0, i, INT32_MAX, bestScoreOut);
			setAllCellsOnDirection(map, &result, xPos, yPos, i, 0, INT32_MAX, bestScoreOut);
		}
		break;
	case FigureType::Queen:
		for (int i = -1; i <= 1; i += 1) {
			for (int j = -1; j <= 1; j += 1) {
				if (i == 0 && j == 0)
					continue;
				setAllCellsOnDirection(map, &result, xPos, yPos, i, j, INT32_MAX, bestScoreOut);
			}
		}
		break;
	case FigureType::King:
		for (int i = -1; i <= 1; i += 1) {
			for (int j = -1; j <= 1; j += 1) {
				if (i == 0 && j == 0)
					continue;
				setAllCellsOnDirection(map, &result, xPos, yPos, i, j, 1, bestScoreOut);
			}
		}

		//Castling
		if (!ignoreCastling) {
			if (figure->totalMoves == 0) {
				if (!(*map)[xPos - 1][yPos] && !(*map)[xPos - 2][yPos] && !(*map)[xPos - 3][yPos] && (*map)[xPos - 4][yPos] && (*map)[xPos - 4][yPos]->type == FigureType::Rook &&
					!cellsUnderAttack(map, { xPos - 1, xPos - 2, xPos - 3 }, { yPos , yPos,yPos }, !player)) {
					result[xPos - 2][yPos] = evaluateCurrentMove(map, xPos, yPos, xPos - 3, yPos, player);
					if (bestScoreOut && (*bestScoreOut) < result[xPos - 2][yPos])
						(*bestScoreOut) = result[xPos - 2][yPos];
				}
				if (!(*map)[xPos + 1][yPos] && !(*map)[xPos + 2][yPos] && (*map)[xPos + 3][yPos] && (*map)[xPos + 3][yPos]->type == FigureType::Rook &&
					!cellsUnderAttack(map, { xPos + 1, xPos + 2 }, { yPos , yPos }, !player)) {
					result[xPos + 2][yPos] = evaluateCurrentMove(map, xPos, yPos, xPos + 2, yPos, player);
					if (bestScoreOut && (*bestScoreOut) < result[xPos + 2][yPos])
						(*bestScoreOut) = result[xPos + 2][yPos];
				}
			}
		}


		break;
	}

	return result;
}

void setAllCellsOnDirection(std::vector<std::vector<Figure*>>* map, std::vector<std::vector<int>>* cells, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int maxMoves, int* bestScoreOut)
{
	Figure* figure = (*map)[xPos][yPos];
	for (int i = 1, X = xPos + xDest, Y = yPos + yDest; i <= maxMoves && validateCoords(X, Y); i++, X += xDest, Y += yDest) {
		Figure* dstFigure = (*map)[X][Y];

		if (dstFigure != nullptr) {
			if (dstFigure->side != figure->side) {
				(*cells)[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, figure->side);
				if (bestScoreOut && (*bestScoreOut) < (*cells)[X][Y])
					(*bestScoreOut) = (*cells)[X][Y];
			}
			break;
		}
		else {
			(*cells)[X][Y] = evaluateCurrentMove(map, xPos, yPos, X, Y, figure->side);
			if (bestScoreOut && (*bestScoreOut) < (*cells)[X][Y])
				(*bestScoreOut) = (*cells)[X][Y];
		}
	}
}

bool validateCoords(size_t xPos, size_t yPos)
{
	return (xPos < 8 && yPos < 8 && xPos >= 0 && yPos >= 0);
}

bool cellsUnderAttack(std::vector<std::vector<Figure*>>* map, std::vector<size_t> xPos, std::vector<size_t> yPos, int enemy)
{
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			//foreach figure find moves
			if ((*map)[x][y] && (*map)[x][y]->side == enemy) {
				std::vector<std::vector<int>> moves = getPossibleMoves(map, x, y, enemy, nullptr, true);
				for (int xDst = 0; xDst < 8; xDst++) {
					for (int yDst = 0; yDst < 8; yDst++) {
						if (moves[xDst][yDst] != -INT32_MAX) {
							
							for (int i = 0; i < xPos.size(); i++) {
								if (xPos[i] == xDst && yPos[i] == yDst)
									return true;
							}

						}
					}
				}
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Position evaluation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int evaluateCurrentMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int player)
{
	Figure* srcFigure = (*map)[xPos][yPos];
	Figure* destFigure = (*map)[xDest][yDest];

	int score = 0;

	if (destFigure) {
		score = beatScore[destFigure->type];
	}

	switch (srcFigure->type)
	{
	case FigureType::Pawn:
		score += (pawnMoveScore[abs(7 * player - (int)xDest)][abs(7 * player - (int)yDest)] - pawnMoveScore[abs(7 * player - (int)xPos)][abs(7 * player - (int)yPos)]);
		break;
	case FigureType::Bishop:
		score += (bishopMoveScore[abs(7 * player - (int)xDest)][abs(7 * player - (int)yDest)] - bishopMoveScore[abs(7 * player - (int)xPos)][abs(7 * player - (int)yPos)]);
		break;
	case FigureType::Knight:
		score += (knightMoveScore[abs(7 * player - (int)xDest)][abs(7 * player - (int)yDest)] - knightMoveScore[abs(7 * player - (int)xPos)][abs(7 * player - (int)yPos)]);
		break;
	case FigureType::Rook:
		score += (rookMoveScore[abs(7 * player - (int)xDest)][abs(7 * player - (int)yDest)] - rookMoveScore[abs(7 * player - (int)xPos)][abs(7 * player - (int)yPos)]);
		break;
	case FigureType::Queen:
		score += (queenMoveScore[abs(7 * player - (int)xDest)][abs(7 * player - (int)yDest)] - queenMoveScore[abs(7 * player - (int)xPos)][abs(7 * player - (int)yPos)]);
		break;
	case FigureType::King:
		score += (kingEarlyMoveScore[abs(7 * player - (int)xDest)][abs(7 * player - (int)yDest)] - kingEarlyMoveScore[abs(7 * player - (int)xPos)][abs(7 * player - (int)yPos)]);
		break;
	}

	if (score > 35000 || score < -35000) {
		throw "Something happend to score evaluation, score:" + score;
	}

	return score;
}

void simulateMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest)
{
	//Castling
	if ((*map)[xPos][yPos]->type == King && (*map)[xPos][yPos]->totalMoves == 0) {
		if ((xDest == xPos + 2)) {
			(*map)[xPos + 1][yPos] = (*map)[xPos + 3][yPos];
			(*map)[xPos + 3][yPos] = nullptr;
			(*map)[xPos + 1][yPos]->totalMoves++;
		}
		else if (xDest == xPos - 2) {
			(*map)[xPos - 2][yPos] = (*map)[xPos - 4][yPos];
			(*map)[xPos - 4][yPos] = nullptr;
			(*map)[xPos - 2][yPos]->totalMoves++;
		}
	}

	(*map)[xDest][yDest] = (*map)[xPos][yPos];
	(*map)[xPos][yPos] = nullptr;
	(*map)[xDest][yDest]->totalMoves++;
}

void unSimulateMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest)
{
	//Castling
	if ((*map)[xPos][yPos]->type == King && (*map)[xPos][yPos]->totalMoves == 0) {
		if ((xDest == xPos + 2)) {
			(*map)[xPos + 3][yPos]->totalMoves--;
		}
		else if (xDest == xPos - 2) {
			(*map)[xPos - 4][yPos]->totalMoves--;
		}
	}

	(*map)[xPos][yPos]->totalMoves--;
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

bool FilterUserMoves(std::vector<std::vector<Figure*>>* map, std::vector<std::vector<int>>* moves, int xPos, int yPos, int player)
{
	bool noMovesRemain = true;
	for (int xDst = 0; xDst < 8; xDst++) {
		for (int yDst = 0; yDst < 8; yDst++) {
			if ((*moves)[xDst][yDst] != -INT32_MAX) {
				std::vector<std::vector<Figure*>> newMap = *map;

				simulateMove(&newMap, xPos, yPos, xDst, yDst);
				std::vector<int> moveRes = selectBestMove(nullptr, &newMap, !player, 1, 0, false);
				unSimulateMove(map, xPos, yPos, xDst, yDst);

				if (moveRes[4] >= beatScore[0] * 0.75) {
					(*moves)[xDst][yDst] = -INT32_MAX;
				}
				else {
					noMovesRemain = false;
				}
			}
		}
	}
	return noMovesRemain;
}

/*	std::vector<std::vector<Figure*>> new_map = {{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
												{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr}, };*/