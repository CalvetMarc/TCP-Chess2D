#include "BoardSquare.h"
#include "ChessPiece.h"

BoardSquare::BoardSquare(const sf::Color color, Vector2 newPos) : defaultSquareColor(color), pos(newPos)
{
	square.setSize(sf::Vector2f(GAMEWIDTH / 8.0f, GAMEHEIGTH / 8.0f));                                   //Al crear la casella li fiquem el tamany la posicio i el color
	square.setPosition(newPos.column * square.getSize().y, newPos.row * square.getSize().x);
	square.setFillColor(defaultSquareColor);
}

BoardSquare::~BoardSquare()
{
	if (piecePlaced == nullptr)
		delete piecePlaced;
}

void BoardSquare::SetPiece(ChessPiece* newPiece, bool startSetting)
{
	piecePlaced = newPiece;

	if (newPiece == nullptr)            //Si la peça que ens han donat no es nula li actualitzem la posicio al tablero i a la finestra
		return;

	piecePlaced->SetPos(pos);
	piecePlaced->SetPosInWindow(square.getPosition());

	if (!startSetting && !piecePlaced->GetFirstMoveDone())   //Si el moviment no es el inicial per colocar les fitxes i la ficha que ha canviat la posicio es el primer cop que es mou la marquem com que ya sha mogut
		piecePlaced->SetFirstMoveDone(true);
}

void BoardSquare::Draw(sf::RenderWindow* gameWindow)
{
	gameWindow->draw(square);                      //Pintem la casella amb el color que tingui i despres li canviem el color al inicial per si no se li fa cap canvi de color que ya estigui preparada
	square.setFillColor(defaultSquareColor);
	
	if (piecePlaced != nullptr)               //Si la casella te una peça tambe la pintem
		piecePlaced->Draw(gameWindow);
}
