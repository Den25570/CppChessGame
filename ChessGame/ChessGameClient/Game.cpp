#include "Game.hpp"

Game::Game()
{
	this->CurrentGameState = MoveState::InMenu;

	//ToDo: select in client
	this->Player1 = PlayerType::User;
	this->Player2 = PlayerType::AI;

	this->CurrentActiveSide = 0;
}

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

void Game::ResetMove(bool updateMaps)
{
	std::vector<int> move = this->logger.ResetMove();
	this->board.figures[move[0]][move[1]] = this->board.figures[move[2]][move[3]];
	this->board.figures[move[0]][move[1]]->totalMoves--;
	if (move.size() == 5) {
		this->board.figures[move[2]][move[3]] = this->board.deadFigures[this->board.deadFigures.size() - 1];
		this->board.deadFigures.pop_back();
	}
	else {
		this->board.figures[move[2]][move[3]] = nullptr;
	}
	if (updateMaps) {
		this->board.SetAllPossibleMoves(this->CurrentActiveSide);
		this->board.GetFiguresAttackingKing(this->CurrentActiveSide);
	}
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
		this->board.figuresAttackingKing.clear();

		if (move.size() != 3) {
			this->logger.AddMove(&(this->board.figures), std::vector<int> {this->board.selectedCell.X, this->board.selectedCell.Y, move[0], move[1]});
		}
		else {
			this->logger.AddMove(&(this->board.figures), std::vector<int> {this->board.selectedCell.X, this->board.selectedCell.Y, move[0], move[1], move[2]});
		}
		
		PassMoveToNextPlayer();

		return true;
	}
	return false;
}

void Game::AIMove()
{
	this->board.figuresAttackingKing.clear();
	std::vector<int> move = this->board.AIMove(this->CurrentActiveSide);
	this->logger.AddMove(&(this->board.figures), move);

	PassMoveToNextPlayer();
}

void Game::PassMoveToNextPlayer()
{
	this->CurrentActiveSide = !this->CurrentActiveSide;
	this->CurrentGameState = MoveState::WaitForMove;

	this->board.SetAllPossibleMoves(this->CurrentActiveSide);
	this->board.GetFiguresAttackingKing(this->CurrentActiveSide);
}

