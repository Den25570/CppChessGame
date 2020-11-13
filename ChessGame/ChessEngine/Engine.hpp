#pragma once

#include <vector>
#include "Figure.hpp"

struct Point {
	int x;
	int y;
};

Point selectBestMove(std::vector<std::vector<Figure*>>* map, int player, size_t depth);

//1. Move generation
std::vector<std::vector<int>> getPossibleMoves(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, int player);
//2. Position evaluation
int evaluateCurrentMove(std::vector<std::vector<Figure>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int player);
//3. Minmax tree build
void simulateMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest);

//utils
Point getMaxValue(std::vector<std::vector<int>>* values);