#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <set>
#include "Structures.h"

class BoardSquare;

class ChessPiece
{
public:
	ChessPiece(bool _white, sf::Sprite spriteSheet, PieceType _id);
	Vector2 GetPos() { return pos; }
	void SetPos(Vector2 newPos) { pos = newPos; }
	bool GetIsWhite() { return white; }
	PieceType GetType() { return id; }
	bool GetFirstMoveDone() { return firstMoveDone; }
	void SetFirstMoveDone(bool _firstMoveDone) { firstMoveDone = _firstMoveDone; }
	void SetPosInWindow(sf::Vector2f posIW) { sprite.setPosition(posIW); };
	void Draw(sf::RenderWindow* gameWindow) { gameWindow->draw(sprite); }
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) = 0;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) = 0;
protected:
	PieceType id;
	bool white, firstMoveDone = false;
	Vector2 pos;
	sf::IntRect tilingRect{0,0,64,64};
	sf::Sprite sprite;	
};

std::set<Vector2> CorrectPositionsInDirections(std::map<Vector2, ChessPiece*> gamePieces, ChessPiece*currentPiece, std::vector<Vector2>directions, int maxMovesDistance, bool takeOutPieceInPath);

class Pawn : public ChessPiece {
public:
	Pawn(bool _white, sf::Sprite spriteSheet, PieceType _id);
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) override;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) override;
};

class Rook : public ChessPiece {
public:
	Rook(bool _white, sf::Sprite spriteSheet, PieceType _id);
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) override;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) override;
};

class Knight : public ChessPiece {
public:
	Knight(bool _white, sf::Sprite spriteSheet, PieceType _id);
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) override;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) override;
};

class Bishop : public ChessPiece {
public:
	Bishop(bool _white, sf::Sprite spriteSheet, PieceType _id);
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) override;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) override;
};

class Queen : public ChessPiece {
public:
	Queen(bool _white, sf::Sprite spriteSheet, PieceType _id);
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) override;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) override;
};

class King : public ChessPiece {
public:
	King(bool _white, sf::Sprite spriteSheet, PieceType _id);
	virtual std::set<Vector2> GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces) override;
	virtual std::set<Vector2> GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces) override;
	bool KingWouldBeUnprotected(std::map<Vector2, ChessPiece*> gamePieces);
};