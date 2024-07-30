#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Structures.h"

class ChessPiece;

class BoardSquare
{
public:
	BoardSquare(const sf::Color color, Vector2 newPos);
	~BoardSquare();
	ChessPiece* GetPiece() { return piecePlaced; }
	void SetPiece(ChessPiece* newPiece, bool startSetting = false);
	sf::Color GetCurrentColor() { return currentSquareColor; }
	void SetCurrentColor(sf::Color newColor) { currentSquareColor = newColor; square.setFillColor(newColor); }
	sf::Vector2f GetPosInWindow() { return square.getPosition(); }
	void Draw(sf::RenderWindow* gameWindow);
private:
	sf::Color defaultSquareColor, currentSquareColor;
	sf::RectangleShape square;
	ChessPiece* piecePlaced = nullptr;
	Vector2 pos;
};

