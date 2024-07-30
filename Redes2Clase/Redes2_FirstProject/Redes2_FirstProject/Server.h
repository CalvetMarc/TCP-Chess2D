#pragma once
#include "Structures.h"
#include <map>
#include "mutex"

class Chat;
class SocketsManager;
class TcpSocket;

class Server
{
public:
	static Server* GetInstance();
	void Run(unsigned short _port, sf::RenderWindow* appWindow, sf::Font* font);
private:
	static Server* instance;
	bool* serverState;
	std::mutex roomsIdMutex, allRoomsMutex, playersInLobbyMutex, clientsInRoomMutex;
	int roomsId = 1;
	sf::IpAddress* ipAddress;
	SocketsManager* sM;
	sf::Font* font;
	Chat* chat;
	sf::RenderWindow* appWindow;
	unsigned short port;
	RoomsUpdateData* allRooms;
	std::map<int, std::pair<std::string, TcpSocket*>> playersInLobby;
	std::map<int, int> clientsInRoom;

	std::vector < std::pair<bool*, std::thread*>> threadsFlags;

	Server();
	void HandleServerState();

};

