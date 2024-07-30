#include "TcpListener.h"
#include "iostream";

bool TcpListener::Listen(unsigned short port, sf::IpAddress address)
{
    std::cout << address.toString();
    return listen(port, address) == sf::Socket::Done;
}

bool TcpListener::Accept(TcpSocket& socket)
{
    
    return accept(socket) ==sf::Socket::Done;
}
