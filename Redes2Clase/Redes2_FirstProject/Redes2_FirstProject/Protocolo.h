#pragma once
#include "Structures.h"
#include <iostream>
#include <vector>

//PacketKey = 0
std::string name;

class CreateRoomRequest { //Player name vol partida

	//PacketKey = 1

public:

	std::string name;

};

class CreateRoomResponse { //Id de la sala a la que sha unit

	//PacketKey = 2

public:

	unsigned long id;

};

class RoomData {

public:

	unsigned long id;
	std::string name;
	__time32_t timestamp;


};

class RoomsUpdate {

	//PacketKey = 3

public:

	std::vector<RoomData> rooms;

};


//PacketKey = 4
unsigned long idRoom;


class Address { //Ip i port on tothom de la sala escoltara i nom de la sala

	std::string ip;
	unsigned short port;
	std::string name;
};

class OnEnterAsPlayer {

	//PacketKey = 5

	std::vector<Address> otherP2PClients;
	bool isFirst;

};


//PacketKey = 6 -> void


//PacketKey = 7
unsigned long idRoom;


class OnEnterAsSpectator {

	//PacketKey = 8

	std::vector<Address> otherP2PClients;

};

//PacketKey = 9 -> void


//PacketKey = 10
unsigned long idRoom;

//PacketKey = 11
bool isReady;

//PacketKey = 12 -> void


////////////////////Gameplay

class Vector2 {
	unsigned short x, y;
};

//PacketKey = 0
Vector2 selectedPosition;

//PacketKey = 1 -> void

//PacketKey = 2
Vector2 targetPosition;

enum PieceType {
	Pawn = 0,
	Knight = 1,
	Bishop = 2,
	Tower = 3,
	Queen = 4,
	King = 5
};

//PacketKey = 3
class Promotion {
	Vector2 targetPosition;
	PieceType piece;
};


//PacketKey = 4
class Castle {
	Vector2 kingTargetPosition;
	Vector2 towerTargetPosition;
	Vector2 towerCurrentPosition;
};


//PacketKey = 5
class EnPassant {
	Vector2 targetPosition;
	Vector2 pawnKilledPosition;
};