#include "SocketsManager.h"
#include <iostream>

SocketsManager::SocketsManager(OnSocketConnected onSocketConnected)
{
	_OnSocketConnected = onSocketConnected;

}

SocketsManager::~SocketsManager()
{
	delete _listener;
	for (TcpSocket* socket : _sockets)
	{
		delete socket;
	}
}

void SocketsManager::StartLoop()
{
	_isRunningMutex.lock();

	if (_isRunning)
	{
		_isRunningMutex.unlock();
		return;
	}

	_isRunning = true;
	_isRunningMutex.unlock();

	std::thread* loopThread = new std::thread(&SocketsManager::SelectorLoop, this); //Comencem el bucle del selector perque vagi gestion els packets i conexions que es reben
	loopThread->detach();
}

bool SocketsManager::StartListener(unsigned short port, sf::IpAddress address)
{
	_listenerMutex.lock();

	if (_listener != nullptr)
	{
		_listenerMutex.unlock();
		return false;
	}

	_listener = new TcpListener(); //Inicialitzem el listener per poder començar a escoltar conexions
	if (!_listener->Listen(port, sf::IpAddress::Any))
	{
		delete _listener;
		_listenerMutex.unlock();
		return false;
	}

	_selector.Add(*_listener); //Afegim el listener al selector pq gestioni les coneixon request

	_listenerMutex.unlock(); 

	return true;
}

bool SocketsManager::EndListener()
{
	return true;

	_listenerMutex.lock();


	if (_listener == nullptr)
	{
		_listenerMutex.unlock();
		return false;
	}

	_listener->close(); //Inicialitzem el listener per poder començar a escoltar conexions

	_selector.Remove(*_listener);

	delete _listener;
	_listener = nullptr;

	_listenerMutex.unlock();

	return true;
}

bool SocketsManager::ConnectToServer(std::string ip, unsigned short port, sf::Time t)
{
	TcpSocket* socket = new TcpSocket(); //Creem un socket per conectarnos

	if (!socket->Connect(ip, port, t))  //Intentem conectarnos a la ip
	{
		delete socket;
		return false;
	}
	
	AddSocket(socket); //Si el podem conectar l'afegim a a llista de sockets que te el selector

	return true; //false
}

void SocketsManager::SendAll(Packet::PacketKey key, Packet p)
{
	_socketsMutex.lock();

	for (TcpSocket* socket : _sockets)
	{
		if (socket->Send(key, p)) {
			std::cout << "Sended to " << socket->getRemoteAddress().toString() << std::endl;
		}
		else {
			std::cout << "Error sended to " << socket->getRemoteAddress().toString() << std::endl;
		}
	}
	_socketsMutex.unlock();
}

void SocketsManager::SelectorLoop() //Bucle que revisa el selector
{
	_isRunningMutex.lock();
	bool isRunning = true;
	_isRunningMutex.unlock();


	while (isRunning)
	{
		if (_selector.Wait()) //Si el selector te algun TcpSocket o TcpListener amb un informacio a rebre
		{
			CheckListener(); //Revisar si es el listener qui ha de rebre info

			CheckSockets(); //Revisar si es algun dels sockets qui ha de rebre info
		}

		_isRunningMutex.lock();
		 isRunning = _isRunning;
		_isRunningMutex.unlock();

	}

}

void SocketsManager::CheckListener()
{
	_listenerMutex.lock();

	std::cout << "He passat per checklistener" << std::endl;

	if (_listener != nullptr && _selector.IsReady(*_listener))
	{
		TcpSocket* socket = new TcpSocket(); 
		if (_listener->Accept(*socket)) //Si el listener te una conexio pendent se li fa un TcpSocket y es guarda dins la llista de TCpSockets
		{
			AddSocket(socket);
			std::cout << "Conexion accepted" << std::endl;
		}
		else
		{
			delete socket;
		}
	}

	_listenerMutex.unlock();

}

void SocketsManager::CheckSockets()
{
	_socketsMutex.lock();

	for (TcpSocket* socket : _sockets)
	{
		if (_selector.IsReady(*socket)) //Es revisa quin es el socket que ha de rebre un packet
		{
			socket->Receive();
		}
	}
	_socketsMutex.unlock();

}

//int SocketsManager::GetSocketID(TcpSocket* socket)
//{
//	int idToReturn = -1;
//	_socketsMutex.lock();
//	for (auto it = _sockets.begin(); it != _sockets.end(); it++) {
//		if (it->second == socket) {
//			idToReturn = it->first; //Treiem el socket de la llista pq ya no rebra mes packets
//			break;
//		}
//	}
//	_socketsMutex.unlock();
//	return idToReturn;
//}

void SocketsManager::AddSocket(TcpSocket* socket)
{
	_socketsMutex.lock();

	playersIdMutex.lock();
	int newPlayerId = playersId;
	playersId++;
	playersIdMutex.unlock();

	_sockets.push_back(socket); //S'afegeix el TCPSocket a la llista de TCPSocket

	_selector.Add(*socket); //S'afegeix el TCPSocket a la llista interna que te SocketSelect

	std::cout << "He passat per addsocket" << std::endl;


	_OnSocketConnected(newPlayerId, socket); //S'executa la lambda que hem guardat al cosntrcutor passant el nou socket conectat


	socket->SubscribeOnDisconnect([this](TcpSocket* socket) { //S'afegeix una lamda mes a la llista de lambdas que s'executen al desoncetar el TCPSocket
		RemoveSocketAsync(socket);
	});


	_socketsMutex.unlock();
}

void SocketsManager::RemoveSocket(TcpSocket* socket)
{

	_selector.Remove(*socket); //Treiem el socket del selector pq ya no hem de revisar si rep mes packets


	_socketsMutex.lock();

	_sockets.remove(socket);

	_socketsMutex.unlock();
	delete socket;
}

void SocketsManager::RemoveSocketAsync(TcpSocket* socket)
{
	std::thread* removeSocketThread = new std::thread(&SocketsManager::RemoveSocket, this, socket);
	removeSocketThread->detach();
}
