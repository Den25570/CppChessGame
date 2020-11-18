#pragma once
#include <vector>
#include <string>
#include <Figure.hpp>

class Logger {
public:
	std::vector<std::wstring> log;

	void AddMove(std::vector<std::vector<Figure*>>* map, std::vector<int> move);
	void AddMove(Figure* selectedFigure, std::vector<int> move);
};