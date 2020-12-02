#pragma once
#include <vector>
#include <string>
#include <Figure.hpp>

class Logger {
public:
	std::vector<std::wstring> log;
	std::vector<std::vector<int>> extendedLog;

	wchar_t GetUnicodeFigureRepresentation(FigureType figureType, int side);
	void AddMove(MoveType moveType, std::vector<std::vector<Figure*>>* map, std::vector<int> move);
	std::vector<int> ResetMove();
};