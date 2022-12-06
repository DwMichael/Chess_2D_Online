#include <iostream>
#include <ctype.h>
#include <string>
#include <cstring>
#include <math.h>

#include "chess.h"

ChessEngine::ChessEngine() {
	if (!LoadFEN(startingFEN)) {
		std::cout << "error while loading defualt FEN!\n";
	}
}

void ChessEngine::clearState() {
	memset(board, 0, sizeof(board));
	player = Player::White;
	castles = canCastleNone;
	enPassant = -1; // -1 means no possible en passant, otherwise it's index of square.

	halfmoves = 0;
	fullmoves = 0;
}

bool ChessEngine::LoadFEN(std::string fen) {
	clearState();
	int i = 0;
	int pos = 0;
	std::string pieces = "PNBRQKpnbrqk";
	for (; pos < 64 && i < fen.length(); i++) {
		if (pieces.find(fen[i]) != std::string::npos)
			board[7 - pos / 8][pos++ % 8] = fen[i];
		else if ('0' <= fen[i] && fen[i] <= '8')
			pos += fen[i] - '0';
		else if (fen[i] == '/')
			continue;
		else
			return false;
	}

	if (i >= fen.length() || fen[i] != ' ' || pos != 64)
		return false;

	if (++i < fen.length()) {
		if (fen[i] == 'w')
			player = Player::White;
		else if (fen[i] == 'b')
			player = Player::Black;
		else
			return false;
	}

	i += 2;
	if (i >= fen.length())
		return false;

	while (i < fen.length()) {
		switch (fen[i++]) {
		case 'K':
			castles.K = true; break;
		case 'Q':
			castles.Q = true; break;
		case 'k':
			castles.k = true; break;
		case 'q':
			castles.q = true; break;
		case '-':
			continue;
		case ' ':
			goto fen_end_checking_castles;
		default:
			return false;
		}
	}
	fen_end_checking_castles:;

	if (fen[i] == '-') {
		enPassant = -1;
	}
	else if (++i < fen.length() &&
		'a' <= fen[i - 1] && fen[i - 1] <= 'h' &&
		'1' <= fen[i] && fen[i] <= '8') {
		enPassant = (8 - fen[i] - '1') * 8 + fen[i - 1] - 'a';
	}
	else
		return false;

	i += 2;
	if (i >= fen.length())
		return false;

	int space = fen.find(' ', i);
	if (space == std::string::npos)
		return false;

	try {
		halfmoves = std::stoi(fen.substr(i, space));
		fullmoves = std::stoi(fen.substr(space));
	}
	catch (...) {
		return false;
	}

	return true;
}


void ChessEngine::printBoard() {
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 0; file < 8; file++) {
			if (board[rank][file] == '\0')
				std::cout << "[ ]";
			else
				std::cout << '[' << board[rank][file] << ']';
		}
		std::cout << '\n';
	}
}

bool ChessEngine::isCheck() {
	char piece, queen;
	for (int i = 0; i < 64; i++) {
		if (board[0][i] != (player == Player::White ? 'K' : 'k'))
			continue;

		// check knights
		piece = (player == Player::White ? 'n' : 'N');
		int positions[] = { -17, -15, -10, -6, 6, 10, 15, 17 };
		for (int pi : positions)
			if (InBounds(i + pi) && board[0][i + pi] == piece)
				return true;

		// check rooks and queens
		piece = (player == Player::White ? 'r' : 'R');
		queen = (player == Player::White ? 'q' : 'Q');
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i + j * 8)) break;
			if (board[0][i + j * 8] == 0) continue;
			if (board[0][i + j * 8] == piece || board[0][i + j * 8] == queen)
				return true;
			break;
		}
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i - j * 8)) break;
			if (board[0][i - j * 8] == 0) continue;
			if (board[0][i - j * 8] == piece || board[0][i - j * 8] == queen)
				return true;
			break;
		}
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i + j)) break;
			if (board[0][i + j] == 0) continue;
			if (board[0][i + j] == piece || board[0][i + j] == queen)
				return true;
			break;
		}
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i - j)) break;
			if (board[0][i - j] == 0) continue;
			if (board[0][i - j] == piece || board[0][i - j] == queen)
				return true;
			break;
		}

		// check bishops and queens
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i + j * 9)) break;
			if (board[0][i + j * 9] == 0) continue;
			if (board[0][i + j * 9] == piece || board[0][i + j * 9] == queen)
				return true;
			break;
		}
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i - j * 9)) break;
			if (board[0][i - j * 9] == 0) continue;
			if (board[0][i - j * 9] == piece || board[0][i - j * 9] == queen)
				return true;
			break;
		}
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i + j * 7)) break;
			if (board[0][i + j * 7] == 0) continue;
			if (board[0][i + j * 7] == piece || board[0][i + j * 7] == queen)
				return true;
			break;
		}
		for (int j = 1; j < 8; j++) {
			if (!InBounds(i - j * 7)) break;
			if (board[0][i - j * 7] == 0) continue;
			if (board[0][i - j * 7] == piece || board[0][i - j * 7] == queen)
				return true;
			break;
		}

		piece = (player == Player::White ? 'k' : 'K');

		if ((InBounds(i + 1) && board[0][i + 1] == piece) ||
			(InBounds(i - 1) && board[0][i - 1] == piece) ||
			(InBounds(i + 8) && board[0][i + 8] == piece) ||
			(InBounds(i - 8) && board[0][i - 8] == piece) ||
			(InBounds(i + 9) && board[0][i + 9] == piece) ||
			(InBounds(i - 9) && board[0][i - 9] == piece) ||
			(InBounds(i + 7) && board[0][i + 7] == piece) ||
			(InBounds(i - 7) && board[0][i - 7] == piece)) {
			return true;
		}

		if (player == Player::White) {
			if ((InBounds(i + 9) && board[0][i + 9] == 'p') ||
				(InBounds(i + 7) && board[0][i + 7] == 'p')) {
				return true;
			}
		}
		else {
			if ((InBounds(i - 9) && board[0][i - 9] == 'P') ||
				(InBounds(i - 7) && board[0][i - 7] == 'P')) {
				return true;
			}
		}

	}

	return false;
}

bool ChessEngine::InBounds(int idx) {
	return 0 <= idx && idx < 64;
}

bool ChessEngine::tryMove(std::string from, std::string to) {
	int fromIdx = (from[0] - 'a') + (from[1] - '1') * 8;
	int toIdx = (to[0] - 'a') + (to[1] - '1') * 8;

	int fileDif = abs(toIdx % 8 - fromIdx % 8);
	int rankDif = abs(toIdx / 8 - fromIdx / 8);
	if (!InBounds(fromIdx) || !InBounds(toIdx))
		return false;

	int sig = copysign(1, toIdx - fromIdx); // direction used in switch case

	char destPiece = board[0][toIdx];
	int prevPassant = enPassant;
	CanCastle prevCastle = castles;

	char piece = board[0][fromIdx];
	if ((isupper(piece) && (player == Player::Black)) || (islower(piece) && (player == Player::White)))
		return false;
    
	if (destPiece != 0 && islower(destPiece) == islower(piece))
		return false;
    
	switch (piece) {
	case 'P':
        
		if ((toIdx == fromIdx + 8 && board[0][toIdx] == 0) ||
			((enPassant == toIdx || islower(board[0][toIdx])) && (toIdx == fromIdx + 7 || toIdx == fromIdx + 9))) {
			break;
		}
		else if (toIdx == fromIdx + 16 && board[0][fromIdx + 8] == 0 && board[0][toIdx] == 0) {
			enPassant = fromIdx + 8;
            break;
		}
		else
			return false;
	case 'p':
		if ((toIdx == fromIdx - 8 && board[0][toIdx] == 0) ||
			((enPassant == toIdx || isupper(board[0][toIdx])) && (toIdx == fromIdx - 7 || toIdx == fromIdx - 9))) {
			break;
		}
		else if (toIdx == fromIdx - 16 && board[0][fromIdx - 8] == 0 && board[0][toIdx] == 0) {
			enPassant = fromIdx - 8;
            break;
		}
		else
			return false;
	case 'N':
	case 'n':
		if (((abs(fromIdx / 8 - toIdx / 8) == 2 && abs(fromIdx % 8 - toIdx % 8) == 1) ||
			(abs(fromIdx / 8 - toIdx / 8) == 1 && abs(fromIdx % 8 - toIdx % 8) == 2))) {
			break;
		}
		return false;
	case 'R':
	case 'r':
	case 'Q':
	case 'q':
		if (fromIdx / 8 == toIdx / 8) {
			for (int j = fromIdx + sig; j != toIdx; j += sig) {
				if (board[0][j] == 0) continue;
				return false;
			}
			break;
		}
		else if (fromIdx % 8 == toIdx % 8) {
			for (int j = fromIdx + sig * 8; j != toIdx; j += sig * 8) {
				if (board[0][j] == 0) continue;
				return false;
			}
			break;
		}
		else if (tolower(piece) != 'q')
			return false;
	case 'B':
	case 'b':
		if (fromIdx / 8 - toIdx / 8 == fromIdx % 8 - toIdx % 8) {
			for (int j = fromIdx + sig * 9; j != toIdx; j += sig * 9) {
				if (board[0][j] == 0) continue;
				return false;
			}
		}
		else if (fromIdx / 8 - toIdx / 8 == toIdx % 8 - fromIdx % 8) {
			for (int j = fromIdx + sig * 7; j != toIdx; j += sig * 7) {
				if (board[0][j] == 0) continue;
				return false;
			}
		}
		else
			return false;
		break;
	case 'K':
	case 'k':
		if ((fileDif == 1 || fileDif == 0) && (rankDif == 1 || rankDif == 0))
			break;
	default:
		return false;
	}


	board[0][toIdx] = piece;
	board[0][fromIdx] = 0;

	if (enPassant == toIdx) {
		if (player == Player::White)
			board[0][toIdx - 8] = 0;
		else
			board[0][toIdx + 8] = 0;
	}

	if (prevPassant == enPassant)
		enPassant = -1;

	if (isCheck()) {
		enPassant = prevPassant;
		castles = prevCastle;
		board[0][fromIdx] = piece;
		board[0][toIdx] = destPiece;

		if (enPassant == toIdx) {
			if (player == Player::White)
				board[0][toIdx - 8] = 'p';
			else
				board[0][toIdx + 8] = 'P';
		}

		return false;
	}

	player = player == Player::White ? Player::Black : Player::White;
	return true;

}


