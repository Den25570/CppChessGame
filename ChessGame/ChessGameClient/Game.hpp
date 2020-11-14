#pragma once

#include "Board.hpp"

enum MoveState {
	WaitForMove,
	MovingFigure,
	Initializing,
	Finishing
};

enum PlayerType {
	User,
	AI
};

class Game {
public:
	MoveState CurrentGameState;
	int CurrentActiveSide;

	PlayerType Player1;
	PlayerType Player2;

	Board board;

	Game() {
		this->CurrentGameState = MoveState::Initializing;

		//ToDo: select in client
		this->Player1 = PlayerType::User;
		this->Player2 = PlayerType::AI;

		this->CurrentActiveSide = 0;
	}

	void InitGame();
	bool TrySelectFigure(Point pos);
	bool TryMove(Point pos);
	void AIMove();
private:

};