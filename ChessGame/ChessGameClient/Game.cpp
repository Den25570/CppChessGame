#include "Game.hpp"

Game::Game()
{
	this->CurrentGameState = MoveState::InMenu;
	this->isMate = false;
	this->CurrentActiveSide = 0;
}

void Game::InitGame(PlayerType Player1, PlayerType Player2)
{
	this->CurrentGameState = MoveState::Initializing;

	this->Player1 = Player1;
	this->Player2 = Player2;

	this->board.InitGame();
	if (Player1 == PlayerType::User)
		this->board.SetAllPossibleMoves(0);
	if (Player2 == PlayerType::User)
		this->board.SetAllPossibleMoves(1);

	this->CurrentActiveSide = 0;
	this->isMate = false;
	this->CurrentGameState = MoveState::WaitForMove;
	this->logger.log.clear();
	this->logger.extendedLog.clear();
}

void Game::ResetMove(bool updateMaps)
{
	std::vector<int> move = this->logger.ResetMove();
	if (!move.empty()) {
		this->board.figures[move[0]][move[1]] = this->board.figures[move[2]][move[3]];
		this->board.figures[move[0]][move[1]]->totalMoves--;

		//reset castling
		if (board.figures[move[2]][move[3]]->type == King && board.figures[move[2]][move[3]]->totalMoves == 0) {
			if ((move[2] == move[0] + 2)) {
				board.figures[move[0] + 3][move[1]] = board.figures[move[0] + 1][move[1]];
				board.figures[move[0] + 3][move[1]]->totalMoves--;
				board.figures[move[0] + 1][move[1]] = nullptr;
			}
			else if (move[2] == move[0] - 3) {
				board.figures[move[0] - 4][move[1]] = board.figures[move[0] - 2][move[1]];
				board.figures[move[0] - 4][move[1]]->totalMoves--;
				board.figures[move[0] - 2][move[1]] = nullptr;
			}
		}

		//reset move
		if (move.size() == 5) {
			this->board.figures[move[2]][move[3]] = this->board.deadFigures[this->board.deadFigures.size() - 1];
			this->board.deadFigures.pop_back();
		}
		else {
			this->board.figures[move[2]][move[3]] = nullptr;
		}

		this->CurrentActiveSide = !this->CurrentActiveSide;
		this->CurrentGameState = MoveState::WaitForMove;
		if (updateMaps) {
			this->board.SetAllPossibleMoves(this->CurrentActiveSide);
			this->board.GetFiguresAttackingKing(this->CurrentActiveSide);
		}
	}

}

bool Game::TrySelectFigure(Point pos)
{
	if (this->CurrentGameState != InMenu) {
		if ((this->CurrentActiveSide == 0 && this->Player1 == PlayerType::User) || (this->CurrentActiveSide == 1 && this->Player2 == PlayerType::User))
			return this->board.TrySelectFigure(pos, this->CurrentActiveSide);
	}	
	return false;
}

MoveType Game::TryMove(Point pos)
{
	std::vector<int> move;
	MoveType moveType = this->board.TryMove(pos, &move);
	if (moveType != None) {
		this->board.figuresAttackingKing.clear();

		if (move.size() != 3) {
			this->logger.AddMove(moveType, &(this->board.figures), std::vector<int> {this->board.selectedCell.X, this->board.selectedCell.Y, move[0], move[1]});
		}
		else {
			this->logger.AddMove(moveType, &(this->board.figures), std::vector<int> {this->board.selectedCell.X, this->board.selectedCell.Y, move[0], move[1], move[2]});
		}
		
		PassMoveToNextPlayer();
	}
	return moveType;
}

MoveType Game::AIMove()
{
	this->board.figuresAttackingKing.clear();
	std::vector<int> move;
	MoveType moveType = this->board.AIMove(this->CurrentActiveSide, difficulty, &move);
	this->logger.AddMove(moveType, &(this->board.figures), move);

	PassMoveToNextPlayer();

	return moveType;
}

void Game::PassMoveToNextPlayer()
{
	this->CurrentActiveSide = !this->CurrentActiveSide;
	this->CurrentGameState = MoveState::WaitForMove;
	this->board.SetAllPossibleMoves(this->CurrentActiveSide);
	this->board.GetFiguresAttackingKing(this->CurrentActiveSide);
	this->isMate = this->board.isMate;
}

