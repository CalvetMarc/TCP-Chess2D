#pragma once
#include <iostream>
#include "Structures.h"

class Chat;
class ChessManager;
class SocketsManager;

class Client
{
public:
	static Client* GetInstance();
	void Run(unsigned short port, sf::RenderWindow* appWindow, sf::Font* font);
private:
	static Client* instance;
	//std::vector< std::function<void(TcpSocket* socket)>> mainThreadUI;
	//std::atomic<bool>* conexionDone, *inRoom;
	std::string* name;
	SocketsManager* sM;
	std::atomic<ClientState>* currentState;
	std::mutex* lobbyMutex, allMssgMutex;
	RoomsUpdateData* lobbyInfo;
	Chat* chat;
	sf::RenderWindow* appWindow;
	sf::Font* font;
	TcpSocket* serverSocket;
	std::atomic <sf::Uint64>* currentSeed;

	CPVector<Address> clientsForP2P;
	bool isFirst;
	std::atomic<bool>* gameReady;

	std::atomic<sf::Uint64>* currentRoom;

	CPList<ChatMessage>*allMessages;

	Client();
	void StartLogIn();
	void Lobby();
	void InGame();
};

