#include "Logger.hpp"

void Logger::AddMove(std::vector<std::vector<Figure*>>* map, std::vector<int> move)
{
	std::wstring sMove = L"";
	sMove += wchar_t(move[2] + L'a');
	sMove += wchar_t(move[3] + L'1');

	switch ((*map)[move[2]][move[3]]->type)
	{
	case FigureType::Pawn:
		sMove += (*map)[move[2]][move[3]]->side == 0 ? L'\x2659' : L'\x265f';
		break;
	case FigureType::Bishop:
		sMove += (*map)[move[2]][move[3]]->side == 0 ? L'♗' : L'♝';
		break;
	case FigureType::Knight:
		sMove += (*map)[move[2]][move[3]]->side == 0 ? L'♘' : L'♞';
		break;
	case FigureType::Rook:
		sMove += (*map)[move[2]][move[3]]->side == 0 ? L'♖' : L'♜';
		break;
	case FigureType::Queen:
		sMove += (*map)[move[2]][move[3]]->side == 0 ? L'♕' : L'♛';
		break;
	case FigureType::King:
		sMove += (*map)[move[2]][move[3]]->side == 0 ? L'♔' : L'♚';
		break;
	};

	this->log.push_back(sMove);
}

void Logger::AddMove(Figure* selectedFigure, std::vector<int> move)
{
	std::wstring sMove = L"";
	sMove += wchar_t(move[0] + L'a');
	sMove += wchar_t(move[1] + L'1');

	switch (selectedFigure->type)
	{
	case FigureType::Pawn:
		sMove += selectedFigure->side == 0 ? L'\x2659' : L'\x265f';
		break;
	case FigureType::Bishop:
		sMove += selectedFigure->side == 0 ? L'♗' : L'♝';
		break;
	case FigureType::Knight:
		sMove += selectedFigure->side == 0 ? L'♘' : L'♞';
		break;
	case FigureType::Rook:
		sMove += selectedFigure->side == 0 ? L'♖' : L'♜';
		break;
	case FigureType::Queen:
		sMove += selectedFigure->side == 0 ? L'♕' : L'♛';
		break;
	case FigureType::King:
		sMove += selectedFigure->side == 0 ? L'♔' : L'♚';
		break;
	};

	this->log.push_back(sMove);
}
