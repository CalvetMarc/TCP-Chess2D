#pragma once
#include <iostream>
#include <vector>

class Piece
{
private:
	int arrayOfChess[64];

public:
	Piece(int Chess[]);

	bool isTileOccupied(int tile);

	std::string pieceAlliance(int tile, std::string alliance);

};


class king
{
private:
	const int CANDIDATE_MOVE_COORDINATES[8] = { -9, -8, -7, -1, 1, 8, 7, 9 };
	int pieceTile;
	std::string alliance;
	int arrOfChess[64];

public:
	king(int pieceTile, std::string alliance, const int chess[]);

	bool isValidTileCoordinate(int currentCandidate);

	//-------------------------------EXCLUSIONS

	bool firstColum(int pieceTile, int offset);

	bool eighthColum(int pieceTile, int offset);

	//--------------------EXCLUSIONS

	std::vector<int> getLegalMoves();
};


class knight
{
private:
	const int CANDIDATE_MOVE_COORDINATES[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
	int pieceTile;
	std::string alliance;
	int arrOfChess[64];

public:
	knight(int pieceTile, std::string alliance, int chess[]);

	bool isValidTileCoordinate(int currentCandidate);

	//-------------------------------EXCLUSIONS

	bool firstColum(int pieceTile, int offset);

	bool secondColum(int pieceTile, int offset);

	bool seventhColum(int pieceTile, int offset);

	bool eighthColum(int pieceTile, int offset);

	//--------------------EXCLUSIONS

	std::vector<int> getLegalMoves();
};

class pawn
{
private:
	const int CANDIDATE_MOVE_COORDINATES[4] = { 7, 8, 9, 16 };
	int pieceTile;
	int arrOfChess[64];
	std::string alliance;
	signed int direction;

public:
	pawn(int Tile, std::string alliance, int chess[]);

	bool isValidTileCoordinate(int currentCandidate);

	//------------------------------------------------------------------EXCLUSIONS
	//---------------------------------COLUMN

	bool eighthColum(int tile);

	bool firstColum(int tile);

	bool isFirstMove(int tile);

	//-----------------CALCULATIONS

	std::vector<int> getLegalMoves();

};

class queen
{
private:
	const int CANDIDATE_MOVE_COORDINATES[8] = { -8, -1, 1, 8, -9, -7, 9, 7 };
	int pieceTile;
	std::string alliance;
	int arrOfChess[64]{};

public:
	queen(const int Tile, std::string alliance, int chess[]);

	bool isValidTileCoordinate(int currentCandidate);

	//---------------------------EXCLUSIONS

	bool firstColum(int tile, int offset);

	bool eighthColum(int tile, int offset);

	//---------------------------CALCULATIONS

	std::vector<int> getLegalMoves();

};


class rook
{
private:
	const int CANDIDATE_MOVE_COORDINATES[4] = { -8, -1, 1, 8 };
	int pieceTile;
	std::string alliance;
	int arrOfChess[64];

public:
	rook(const int Tile, std::string alliance, int chess[]);

	bool isValidTileCoordinate(int currentCandidate);

	//---------------------------EXCLUSIONS

	bool firstColum(int tile, int offset);

	bool eighthColum(int tile, int offset);

	//---------------------------CALCULATIONS

	std::vector<int> getLegalMoves();

};

class bishop
{
private:
	const int CANDIDATE_MOVE_COORDINATES[4] = { -9, -7, 9, 7 };
	int pieceTile;
	std::string alliance;
	int arrOfChess[64];

public:
	bishop(int pieceTile, std::string alliance, int chess[]);

	bool isValidTileCoordinate(int currentCandidate);

	//----------------------------Exclusions

	bool firstColum(int pieceTile, int offset);

	bool eighthColum(int pieceTile, int offset);

	//----------------------------CALCULATIONS

	std::vector<int> getLegalMoves();
};

class Identity {
public:
	bool chessKing(int initialTile, int finalTile, std::string alliance, int* arrOfChess);
	bool chessQueen(int initialTile, int finalTile, std::string alliance, int* arrOfChess);
	bool chessPawn(int initialTile, int finalTile, std::string alliance, int* arrOfChess);
	bool chessRook(int initialTile, int finalTile, std::string alliance, int* arrOfChess);
	bool chessBishop(int initialTile, int finalTile, std::string alliance, int* arrOfChess);
	bool chessKnight(int initialTile, int finalTile, std::string alliance, int* arrOfChess);
	std::string checkAlliance(int piece);
	bool identifier(int initialTile, int finalTile, int piece, int* arrOfChess);
public:
};
