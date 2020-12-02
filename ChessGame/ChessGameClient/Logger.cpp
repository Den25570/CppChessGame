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

void Logger::AddMove(MoveType moveType, std::vector<std::vector<Figure*>>* map, std::vector<int> move)
{
	std::wstring sMove = L""; 
	std::vector<int> iMove = move;

	if (moveType == MoveType::Default) {
		sMove += wchar_t(move[0] + L'a');
		sMove += wchar_t(move[1] + L'1');
		sMove += wchar_t(L'-');
		sMove += wchar_t(move[2] + L'a');
		sMove += wchar_t(move[3] + L'1');

		sMove += this->GetUnicodeFigureRepresentation((*map)[move[2]][move[3]]->type, (*map)[move[2]][move[3]]->side);

		if (move.size() == 5) {
			sMove += L'x';
			sMove += this->GetUnicodeFigureRepresentation((FigureType)move[4], !(*map)[move[2]][move[3]]->side);
		}
	}
	else if (moveType == MoveType::LongCastling) {
		sMove += std::wstring(L"O-O-O");
	}
	else if (moveType == MoveType::ShortCastling) {
		sMove += std::wstring(L"O-O");
	}
		
	this->log.push_back(sMove);
	this->extendedLog.push_back(iMove);
}

std::vector<int> Logger::ResetMove()
{
	if (!this->extendedLog.empty()) {
		std::vector<int> move = this->extendedLog[this->extendedLog.size() - 1];
		this->extendedLog.pop_back();
		this->log.pop_back();
		return move;
	}
}
