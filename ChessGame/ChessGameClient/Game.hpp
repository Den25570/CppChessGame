#pragma once

#include "Board.hpp"
#include "Figure.hpp"
#include <vector>

class Game {
public:
	Board board;
	std::vector<Figure> fugures;
};