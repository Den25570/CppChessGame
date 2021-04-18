# CppChessGame
Chess game written with C/C++ and WinApi with custom chess engine and AI.
Game contains two modes: Player vs Player and Player vs AI.
AI difficulty regulated with minimax search depth in main menu.

### Main menu:
![main menu](https://i.ibb.co/4NBcr4K/image.png)

### Chess engine
Chess engine provides complete set of game rules and algorithm for AI.

AI relies on 4 step algoritm:
- Move generation
``` C++
// Move generation for queen
for (int i = -1; i <= 1; i += 1) {
  for (int j = -1; j <= 1; j += 1) {
    if (i == 0 && j == 0)
      continue;
    setAllCellsOnDirection(map, &result, xPos, yPos, i, j, INT32_MAX, bestScoreOut);
  }
}
```
- Position evaluation. Possible score positions
``` C++
int evaluateCurrentMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int player)
{
	Figure* srcFigure = (*map)[xPos][yPos];
	Figure* destFigure = (*map)[xDest][yDest];

	int score = 0;

	if (destFigure) {
		if (destFigure->type == King) {
			return beatScore[destFigure->type];
		}
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
```
- Minimax search tree
``` C++
//Simulating all possible moves
//Then selecting best one
//...

//Simulate one move
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

	//Pawn swap
	if ((*map)[xPos][yPos]->type == Pawn && (yPos == 0 || yPos == 7)) {
		(*map)[xPos][yPos]->type == Queen;
	}

	(*map)[xDest][yDest] = (*map)[xPos][yPos];
	(*map)[xPos][yPos] = nullptr;
	(*map)[xDest][yDest]->totalMoves++;
}
```
- Alpha-beta pruning (same function is used to restrict user moves)
``` C++
bool FilterMoves(std::vector<std::vector<Figure*>>* map, std::vector<std::vector<int>>* moves, int xPos, int yPos, int player)
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

```
