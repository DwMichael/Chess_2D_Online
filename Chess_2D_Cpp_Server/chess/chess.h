#pragma once

//#include <iostream>
//#include <ctype.h>
#include <string>
//#include <math.h>


enum Player : bool {
	White = true,
	Black = false
};

struct CanCastle {
	bool K : 1;
	bool Q : 1;
	bool k : 1;
	bool q : 1;
};

const CanCastle canCastleNone = { false, false, false, false };
const std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class ChessEngine {
private:
	char board[8][8] = { 0 }; // board[rank][file] where rank and file are <0, 7>.
	
	CanCastle castles = canCastleNone;
	int enPassant = -1;

	int halfmoves = 0;
	int fullmoves = 0;

public:
	Player player = Player::White;


	ChessEngine();

	void clearState();

	bool LoadFEN(std::string fen);

	void printBoard();

	bool isCheck();

	bool InBounds(int idx);

	bool tryMove(std::string from, std::string to);
};



