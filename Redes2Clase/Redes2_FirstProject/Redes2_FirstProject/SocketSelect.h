#pragma once
#include "TcpListener.h"

class SocketSelect: public sf::SocketSelector
{
public:

	bool Wait();
	bool IsReady(TcpListener& listener);
	bool IsReady(TcpSocket& socket);

	void Add(TcpListener& listener);
	void Add(TcpSocket& socket);

	void Remove(TcpListener& listener);
	void Remove(TcpSocket& socket);

private:
	std::mutex _mutex;
};

