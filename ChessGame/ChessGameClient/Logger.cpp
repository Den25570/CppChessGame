#include "Logger.hpp"

wchar_t Logger::GetUnicodeFigureRepresentation(FigureType figureType, int side)
{
	switch (figureType)
	{
	case FigureType::Pawn:
		return side == 0 ? L'\x2659' : L'\x265f';
	case FigureType::Bishop:
		return side == 0 ? L'♗' : L'♝';;
	case FigureType::Knight:
		return side == 0 ? L'♘' : L'♞';;
	case FigureType::Rook:
		return side == 0 ? L'♖' : L'♜';
	case FigureType::Queen:
		return side == 0 ? L'♕' : L'♛';
	case FigureType::King:
		return side == 0 ? L'♔' : L'♚';
	};
}

void Logger::AddMove(std::vector<std::vector<Figure*>>* map, std::vector<int> move)
{
	std::wstring sMove = L""; 
	std::vector<int> iMove = move;

	sMove += wchar_t(move[0] + L'a');
	sMove += wchar_t(move[1] + L'1');
	sMove += wchar_t(L'-');
	sMove += wchar_t(move[2] + L'a');
	sMove += wchar_t(move[3] + L'1');

	if ((*map)[move[2]][move[3]]) {
		sMove += this->GetUnicodeFigureRepresentation((*map)[move[2]][move[3]]->type, (*map)[move[2]][move[3]]->side);
		iMove.push_back((*map)[move[2]][move[3]]->type);
	}	
		
	this->log.push_back(sMove);
	this->extendedLog.push_back(iMove);
}
