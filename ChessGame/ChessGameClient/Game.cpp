#include "Game.hpp"

void Game::InitGame()
{
	CurrentGameState = MoveState::Initializing;

	//ToDo: select in client
	PlayerType Player1 = PlayerType::User;
	PlayerType Player2 = PlayerType::AI;	

	this->board.InitGame();
	if (Player1 == PlayerType::User)
		this->board.SetAllPossibleMoves(0);
	if (Player2 == PlayerType::User)
		this->board.SetAllPossibleMoves(1);

	CurrentActiveSide = 0;
	CurrentGameState = MoveState::WaitForMove;
}

bool Game::TrySelectFigure(Point pos)
{
	if ((this->CurrentActiveSide == 0 && this->Player1 == PlayerType::User) || (this->CurrentActiveSide == 1 && this->Player2 == PlayerType::User))
		return this->board.TrySelectFigure(pos, this->CurrentActiveSide);
	else
		return false;
}

bool Game::TryMove(Point pos)
{
	std::vector<int> move;
	if (this->board.TryMove(pos, &move)) {

		this->logger.AddMove(this->board.figures[move[0]][move[1]], move);

		this->CurrentActiveSide = !this->CurrentActiveSide;
		this->CurrentGameState = MoveState::WaitForMove;

		if (this->CurrentActiveSide == PlayerType::User) {
			this->board.SetAllPossibleMoves(this->CurrentActiveSide);
			this->board.GetFiguresAttackingKing(this->CurrentActiveSide);
		}		

		return true;
	}
	return false;
}

void Game::AIMove()
{
	std::vector<int> move = this->board.AIMove(this->CurrentActiveSide);
	this->logger.AddMove(&(this->board.figures), move);

	this->CurrentActiveSide = !this->CurrentActiveSide;
	this->CurrentGameState = MoveState::WaitForMove;

	if (this->CurrentActiveSide == PlayerType::User) {
		this->board.SetAllPossibleMoves(this->CurrentActiveSide);
		this->board.GetFiguresAttackingKing(this->CurrentActiveSide);
	}
		
}

