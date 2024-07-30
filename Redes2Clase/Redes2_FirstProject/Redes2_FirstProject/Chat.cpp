#include "Chat.h"
#include <iostream>
#include <thread>
#include "ConsoleControl.h"

void Chat::ShowMessage(std::string message)
{
	_messageMutex.lock();
	_messages.push_back(message);
	_messageMutex.unlock();

	ConsoleControl::LockMutex();
	std::cout << std::endl << message;
	ConsoleControl::UnlockMutex();
}

void Chat::ShowWarning(std::string message)
{
	ConsoleControl::LockMutex();
	ConsoleControl::SetColor(ConsoleControl::YELLOW);
	std::cout << std::endl << message;
	ConsoleControl::SetColor(ConsoleControl::WHITE);
	ConsoleControl::UnlockMutex();
}

void Chat::ShowError(std::string message)
{
	ConsoleControl::LockMutex();
	ConsoleControl::SetColor(ConsoleControl::RED);
	std::cout << std::endl << message;
	ConsoleControl::SetColor(ConsoleControl::WHITE);
	ConsoleControl::UnlockMutex();
}

void Chat::ListenClientsConnections(unsigned short port)
{
	sf::TcpListener listener;

	if (listener.listen(port) != sf::Socket::Done)
	{
		ShowError("Errorad");
		return;
	}
	sf::IpAddress ipAddress = sf::IpAddress::getLocalAddress();
	ShowWarning("Listening to ip: " + ipAddress.toString());

	while (true != false)
	{
		sf::TcpSocket* client = new sf::TcpSocket();
		sf::Socket::Status status = listener.accept(*client);		
		switch (status)
		{
		case sf::Socket::Done: {
			std::thread clientThread = std::thread(&Chat::OnClientEnter, this,client);
			clientThread.detach();
			clientsIDmutex.lock();
			int id = clientsID;
			clientsID++;
			clientsIdBigger1 = clientsID > 1;
			if (!clientsIdBigger1) {
				room1mutex.lock();
				clientsRoom1.insert(std::make_pair(id, client));
				room1mutex.unlock();
			}
			else {
				room2mutex.lock();
				clientsRoom2.insert(std::make_pair(id, client));
				room2mutex.unlock();
			}
			clientsIDmutex.unlock();
			break;
		}
		case sf::Socket::NotReady:
		case sf::Socket::Partial:
		case sf::Socket::Disconnected:
		case sf::Socket::Error:
		default:
			ShowError("Error on accept Client");
			break;
		}

		if (clientsIdBigger1) {
			listener.close();
			break;
		}
		 
	}


	

}

void Chat::ConnectToServer(std::string ip, unsigned short port)
{
	sf::TcpSocket* socket = new sf::TcpSocket();
	sf::Socket::Status status = socket->connect(ip, port);

	if (CheckError(status, "Error on connecto to Server"))
		return;

	_socketMutex.lock();
	_sockets.push_back(socket);
	_socketMutex.unlock();
	ShowWarning("Connected to server with ip: " + ip);

	std::thread keyboardThread = std::thread(&Chat::ListenKeyboardToSendMessages, this);
	keyboardThread.detach();
	
	std::thread listenThread = std::thread(&Chat::ListenMessages, this, socket);
	listenThread.detach();
}

void Chat::OnClientEnter(sf::TcpSocket* client)
{
	_socketMutex.lock();
	_sockets.push_back(client);
	_socketMutex.unlock();

	ShowWarning("Client accepted IP:" + client->getRemoteAddress().toString());

	ListenMessages(client);
}

void Chat::ListenMessages(sf::TcpSocket* socket)
{
	while (true != false)
	{
		char data[100];
		std::size_t received;
		std::string message;


		if (!CheckError(socket->receive(data, 100, received), "Error receive message"))

			for (size_t i = 0; i < received; i++)
			{
				char c = data[i];
				message += c;
			}
			ShowMessage(message);

			_isServerMutex.lock();
			bool isServer = _isServer;
			_isServerMutex.unlock();

			if (isServer ) {
				SendMessage(message);
			}
	}
}

void Chat::ListenKeyboardToSendMessages()
{
	std::string message = "";

	while (true)
	{
		char c = ConsoleControl::WaitForReadNextChar();

		if (c == KB_Enter)
		{
			SendMessage(message);
			
			_isServerMutex.lock();
			bool isServer = _isServer;
			_isServerMutex.unlock();

			if (isServer) {
				SendMessage(message);
			}

			message = "";
		}
		else 
		{
			message += c;
		}

		/*room1mutex.lock();
		std::cout << clientsRoom1.size();
		room1mutex.unlock();*/

	}
}

Chat* Chat::Server(unsigned short port)
{
	Chat* chat = new Chat();
	chat->_serverAddress = sf::IpAddress::getLocalAddress();
	chat->_isServer = true;

	std::thread listenerThread = std::thread(&Chat::ListenClientsConnections, chat, port);
	listenerThread.detach();

	//Listen keyboard
	//std::thread keyboardThread = std::thread(&Chat::ListenKeyboardToSendMessages, chat);
	//keyboardThread.detach();

	return chat;
}

void Chat::SendMessage(std::string message/*, int room, int id*/)
{

	_socketMutex.lock();
	for (sf::TcpSocket* socket : _sockets)
	{
		CheckError(socket->send(message.c_str(), message.length()), "Error sending message");
	}		
	_socketMutex.unlock();

	/*if (room == 1) {
		room1mutex.lock();
		for (std::map<int, sf::TcpSocket*>::iterator it = clientsRoom1.begin(); it != clientsRoom1.end(); it++) {
			if(it->first != id)
				CheckError(it->second->send(message.c_str(), message.length()), "Error sending message");
		}
		room1mutex.unlock();
	}
	else {
		room2mutex.unlock();
		for (std::map<int, sf::TcpSocket*>::iterator it = clientsRoom2.begin(); it != clientsRoom2.end(); it++) {
			if (it->first != id)
				CheckError(it->second->send(message.c_str(), message.length()), "Error sending message");
		}
		room2mutex.unlock();
	}*/


}

Chat* Chat::Client(std::string ip, unsigned short port)
{
	Chat* chat = new Chat();
	chat->_serverAddress = sf::IpAddress(ip);

	chat->ConnectToServer(ip, port);

	return chat;
}

bool Chat::CheckError(sf::Socket::Status STATUS, std::string error)
{
	if (STATUS != sf::Socket::Done)
	{
		std::cout << std::endl << error;
		return true;
	}
	return false;
}

