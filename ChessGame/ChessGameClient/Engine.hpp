#pragma once

#include <vector>
#include "Figure.hpp"

struct sPoint {
	int X;
	int Y;
};

std::vector<sPoint> selectBestMove(std::vector<std::vector<Figure*>>* map, int player);

//1. Move generation
std::vector<std::vector<int>> getPossibleMoves(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, int player);
void setAllCellsOnDirection(std::vector<std::vector<Figure*>>* map, std::vector<std::vector<int>>* cells, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int maxMoves);
bool validateCoords(size_t xPos, size_t yPos);

//2. Position evaluation
int evaluateCurrentMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest, int player);

//3. Minmax tree build
void simulateMove(std::vector<std::vector<Figure*>>* map, size_t xPos, size_t yPos, size_t xDest, size_t yDest);

//utils
sPoint getMaxValue(std::vector<std::vector<int>>* values);