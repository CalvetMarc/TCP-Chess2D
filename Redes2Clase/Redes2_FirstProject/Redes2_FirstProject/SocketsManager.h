#pragma once

#include <list>

#include "SocketSelect.h"


class SocketsManager
{
public:
	typedef std::function<void(int, TcpSocket* socket)> OnSocketConnected;

	SocketsManager(OnSocketConnected onSocketConnected);
	~SocketsManager();

	void StartLoop();

	bool StartListener(unsigned short port, sf::IpAddress address = sf::IpAddress::Any);
	bool EndListener();
	bool ConnectToServer(std::string ip, unsigned short port, sf::Time t = sf::microseconds(0));
	void SendAll(Packet::PacketKey key, Packet p);
private:

	bool _isRunning = false;
	std::mutex _isRunningMutex;

	SocketSelect _selector;

	TcpListener* _listener;
	std::mutex _listenerMutex, playersIdMutex;
	int playersId = 0;
	std::list<TcpSocket*> _sockets;
	std::mutex _socketsMutex;

	OnSocketConnected _OnSocketConnected;

	void SelectorLoop();
	void CheckListener();
	void CheckSockets();

	//int GetSocketID(TcpSocket* socket);
	void AddSocket(TcpSocket* socket);
	void RemoveSocket(TcpSocket* socket);
	void RemoveSocketAsync(TcpSocket* socket);
};

