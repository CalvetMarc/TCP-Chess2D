#pragma once
#include "TcpSocket.h"

class TcpListener: public sf::TcpListener
{
public:
	bool Listen(unsigned short port, sf::IpAddress address);
	bool Accept(TcpSocket& socket);
};

