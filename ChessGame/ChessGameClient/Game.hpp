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
	int difficulty = 1;

	//utils
	Logger logger;

	Game();

	void InitGame(PlayerType Player1, PlayerType Player2);
	void FinishGame();
	void ResetMove(bool updateMaps);

	bool TrySelectFigure(Point pos);
	MoveType TryMove(Point pos);
	MoveType AIMove();
private:
	void PassMoveToNextPlayer();
};