#pragma once

#include "Board.hpp"
#include "Logger.hpp"

enum MoveState {
	WaitForMove,
	MovingFigure,
	InMenu,
	Initializing,
	Finishing
};

enum PlayerType {
	User,
	AI
};

class Game {
public:
	//Game info
	MoveState CurrentGameState;
	int CurrentActiveSide;
	PlayerType Player1;
	PlayerType Player2;
	Board board;
	bool isMate;

	//utils
	Logger logger;

	Game();

	void InitGame(PlayerType Player1, PlayerType Player2);
	void FinishGame();
	void ResetMove(bool updateMaps);

	bool TrySelectFigure(Point pos);
	bool TryMove(Point pos);
	void AIMove();
private:
	void PassMoveToNextPlayer();
};