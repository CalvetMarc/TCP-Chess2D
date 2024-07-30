#pragma once
#include <string>
#include <mutex>
#include <SFML/Network.hpp>
#include <vector>
#include <list>
#include <map>


class Chat
{
private:
	bool clientsIdBigger1 = false;
	std::map <int, sf::TcpSocket*> clientsRoom1;
	std::map <int, sf::TcpSocket*> clientsRoom2;
	int clientsID = 0;
	std::mutex room1mutex;
	std::mutex room2mutex;
	std::mutex clientsIDmutex;

	bool _isServer = false;
	std::mutex _isServerMutex;
	std::list<sf::TcpSocket*> _sockets;
	std::mutex _socketMutex;

	sf::IpAddress _serverAddress;

	std::vector<std::string> _messages;
	std::mutex _messageMutex;

	Chat() {}
	void ShowMessage(std::string message);
	void ShowWarning(std::string message);
	void ShowError(std::string message);

	void ListenClientsConnections(unsigned short port);
	void ConnectToServer(std::string ip, unsigned short port);


	void OnClientEnter(sf::TcpSocket* client);
	void ListenMessages(sf::TcpSocket* socket);
	void ListenKeyboardToSendMessages();
	void SendMessage(std::string message/*, int room, int id*/);
	bool CheckError(sf::Socket::Status STATUS, std::string error);

public:
	static Chat* Server(unsigned short port);
	static Chat* Client(std::string ip, unsigned short port);
};

	