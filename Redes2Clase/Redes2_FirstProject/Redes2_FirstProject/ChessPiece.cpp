#include "ChessPiece.h"

ChessPiece::ChessPiece(bool _white, sf::Sprite spriteSheet, PieceType _id) : white(_white), id(_id)
{
	sprite = spriteSheet;
	if (white)
		tilingRect.top = 64;	
}

std::set<Vector2> CorrectPositionsInDirections(std::map<Vector2, ChessPiece*> gamePieces, ChessPiece* currentPiece, std::vector<Vector2> directions, int maxMovesDistance, bool takeOutPieceInPath)
{
	std::set<Vector2> correctMoves;

	Vector2 pos = currentPiece->GetPos();

	for (int i = 1; i < maxMovesDistance + 1; i++) {
		for (auto it = directions.begin(); it != directions.end(); ) {
			Vector2 posToCheck = pos + *it * i;
			if (posToCheck.row < 0 || posToCheck.row > 7 || posToCheck.column < 0 || posToCheck.column > 7) { //Si anant cap a una direccio sortim del mapa, la treiem com a direccio disponible
				it = directions.erase(it);
			}
			else {
				auto it2 = gamePieces.find(posToCheck);  //Si anant cap a direccio wns trobem una ficha, la treiem com a direccio disponible 
				if (it2 != gamePieces.end()) {
					if (takeOutPieceInPath && it2->second->GetIsWhite() != currentPiece->GetIsWhite()) { //Si la nostra ficha mata al trobar una altre en la direccio en la que es mou, la posicio es valida
						correctMoves.insert(posToCheck);
					}
					it = directions.erase(it);
				}
				else if (it != directions.end()) {     //Si no ens trobem res anant cap a aquella direccio la poscio es valdia
					correctMoves.insert(posToCheck);
					it++;
				}
			}
		}
	}
	return correctMoves;
}

Pawn::Pawn(bool _white, sf::Sprite spriteSheet, PieceType _id) : ChessPiece(_white, spriteSheet, _id)
{
	tilingRect.left = 320;
	sprite.setTextureRect(tilingRect);
	sprite.setScale(1.3f, 1.3f);
}

std::set<Vector2> Pawn::GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> correctMoves;
	int sign = white ? -1 : 1;
																																			//Es mou vertical, la direccio cap on es mou depen del equip
	correctMoves = CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ Vector2(sign,0) }), firstMoveDone ? 1 : 2, false); //No mata igual que es mou (1 mes lluny si es el primer)

	for (int i = -1; i < 2; i += 2) {                                        
		auto it = gamePieces.find(Vector2(pos + Vector2(sign, sign * i))); //Mirar si podem matar en alguna de las 2 diagonals
		if ((it != gamePieces.end() && it->second->GetIsWhite() != white)) {
			correctMoves.insert(pos + Vector2(sign, sign * i));
		}
	}	

	return correctMoves;
}

std::set<Vector2> Pawn::GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2>moves;

	int sign = white ? -1 : 1;
	Vector2 posDistance = targetPosition - pos;
	if (posDistance == Vector2(sign, sign) || posDistance == Vector2(sign, -sign)) { //Nomes mata si esta a la diagonal a 1 de distancia
		moves.insert({ pos, targetPosition });
	}
	return moves;
}

Rook::Rook(bool _white, sf::Sprite spriteSheet, PieceType _id) : ChessPiece(_white, spriteSheet, _id)
{
	tilingRect.left = 128;
	sprite.setTextureRect(tilingRect);
	sprite.setScale(1.3f, 1.3f);
}

std::set<Vector2> Rook::GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces)
{
	return CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ Vector2(1,0), Vector2(-1,0), Vector2(0,-1), Vector2(0,1) }), ChessSideLength, true); //Es mou en vertical i horizontal
}

std::set<Vector2> Rook::GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> moves;
	Vector2 posDistance = targetPosition - pos;
	if (posDistance.row == 0 || posDistance.column == 0) {
		Vector2 dir = pos.GetDirection(targetPosition);
		moves = CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ dir }), ChessSideLength, true); 
		moves.insert(pos);
		if (moves.find(targetPosition) == moves.end()) { //Si trobem la posicio del target a un dels camins podem matar
			moves.clear();
		}
	}

	return moves;
}

Knight::Knight(bool _white, sf::Sprite spriteSheet, PieceType _id) : ChessPiece(_white, spriteSheet, _id)
{
	tilingRect.left = 192;
	sprite.setTextureRect(tilingRect);
	sprite.setScale(1.3f, 1.3f);
}

std::set<Vector2> Knight::GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> correctMoves;

	correctMoves.insert(pos + Vector2(-1, 2)); //Tots els moviments q pot fer un cavall
	correctMoves.insert(pos + Vector2(-2, 1));
	correctMoves.insert(pos + Vector2(1, 2));
	correctMoves.insert(pos + Vector2(2, 1));
	correctMoves.insert(pos + Vector2(-1, -2));
	correctMoves.insert(pos + Vector2(-2, -1));
	correctMoves.insert(pos + Vector2(1, -2));
	correctMoves.insert(pos + Vector2(2, -1));

	for (auto it = correctMoves.begin(); it != correctMoves.end();) { //Si algun dels moviments surt del tablero el treiem
		if (it->row < 0 || it->row > 7 || it->column < 0 || it->column > 7)
			it = correctMoves.erase(it);
		else if(it != correctMoves.end())
			it++;
	}

	for (auto it1 = correctMoves.begin(); it1 != correctMoves.end();) {
		auto it2 = gamePieces.find(*it1);
		if (it2 != gamePieces.end() && it2->second->GetIsWhite() == white) //Si hi ha alguna ficha aliada a una posicio on podiem mourens la descartem
			it1 = correctMoves.erase(it1);
		else if (it1 != correctMoves.end())
			it1++;
	}

	return correctMoves;
}

std::set<Vector2> Knight::GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> moves;
	Vector2 posDistance = targetPosition - pos;
	bool isValid = posDistance == Vector2(-1, 2) || posDistance == Vector2(-2, 1) || posDistance == Vector2(1, 2) || posDistance == Vector2(2, 1) //Pot matar si esta a algunes de les posicions a la q es mou
		|| posDistance == Vector2(-1, -2) || posDistance == Vector2(-2, -1) || posDistance == Vector2(1, -2) || posDistance == Vector2(2, -1);
	if (isValid)
		moves.insert({ pos, targetPosition }); //Si trobem la posicio del target a un dels camins podem matar
	return moves;
}

Bishop::Bishop(bool _white, sf::Sprite spriteSheet, PieceType _id) : ChessPiece(_white, spriteSheet, _id)
{
	tilingRect.left = 256;
	sprite.setTextureRect(tilingRect);
	sprite.setScale(1.3f, 1.3f);
}

std::set<Vector2> Bishop::GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces)
{
	return CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ Vector2(1, 1), Vector2(-1, -1), Vector2(-1, 1), Vector2(1, -1) }), ChessSideLength, true); //es mou en les diagonals
}

std::set<Vector2> Bishop::GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> moves;
	if (std::abs(pos.row - targetPosition.row) == std::abs(pos.column - targetPosition.column)) {
		Vector2 dir = pos.GetDirection(targetPosition);
		moves = CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ dir }), ChessSideLength, true); 
		moves.insert(pos);
		if (moves.find(targetPosition) == moves.end()) { //Si trobem la posicio del target a un dels camins podem matar
			moves.clear();
		}
	}

	return moves;
}

Queen::Queen(bool _white, sf::Sprite spriteSheet, PieceType _id) : ChessPiece(_white, spriteSheet, _id)
{
	tilingRect.left = 64;
	sprite.setTextureRect(tilingRect);
	sprite.setScale(1.3f, 1.3f);
}

std::set<Vector2> Queen::GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces)
{	
	return CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ Vector2(1,1), Vector2(-1,-1), Vector2(-1, 1), Vector2(1,-1),  //Es mou vertical, horizontal y diagonals
		Vector2(1,0), Vector2(-1,0), Vector2(0,-1), Vector2(0,1) }), ChessSideLength, true);
}

std::set<Vector2> Queen::GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> moves;
	Vector2 posDistance = targetPosition - pos;
	if (posDistance.row == 0 || posDistance.column == 0 || std::abs(pos.row - targetPosition.row) == std::abs(pos.column - targetPosition.column)) {
		Vector2 dir = pos.GetDirection(targetPosition);
		moves = CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ dir }), ChessSideLength, true);
		moves.insert(pos);
		if (moves.find(targetPosition) == moves.end()) { //Si trobem la posicio del target a un dels camins podem matar
			moves.clear();
		}
	}

	return moves;
}

King::King(bool _white, sf::Sprite spriteSheet, PieceType _id) : ChessPiece(_white, spriteSheet, _id)
{
	sprite.setTextureRect(tilingRect);
	sprite.setScale(1.3f, 1.3f);
}

std::set<Vector2> King::GetCorrectMoves(std::map<Vector2, ChessPiece*> gamePieces)
{
	return CorrectPositionsInDirections(gamePieces, this, std::vector<Vector2>({ Vector2(1,1), Vector2(-1,-1), Vector2(-1, 1), Vector2(1,-1), //Es mou vertical horizontal i diagonal (1 de distancia)
		Vector2(1,0), Vector2(-1,0), Vector2(0,-1), Vector2(0,1) }), 1, true);
}

std::set<Vector2> King::GetPathToKillTarget(Vector2 targetPosition, std::map<Vector2, ChessPiece*> gamePieces)
{
	std::set<Vector2> moves;
	Vector2 posDistance = targetPosition - pos;
	bool isValid = std::abs(posDistance.row) <= 1 && std::abs(posDistance.column) <= 1; //Si trobem la posicio del target a un dels camins podem matar
	if (isValid)
		moves.insert({ pos, posDistance });
	return moves;
}

bool King::KingWouldBeUnprotected(std::map<Vector2, ChessPiece*> gamePieces)
{
	return false;
}

