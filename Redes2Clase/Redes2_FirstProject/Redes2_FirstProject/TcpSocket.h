#pragma once
#include <functional>
#include <map>
#include <thread>
#include <mutex>
#include <vector>

#include "Packet.h"


class TcpSocket : public sf::TcpSocket
{
public:

	typedef std::function<void(Packet packet)> OnReceivePacket;
	typedef std::function<void(TcpSocket* socket)> OnSocketDisconnect;

	bool Connect(std::string ip, unsigned short port, sf::Time t = sf::microseconds(0));
	void Receive();
	bool Send(Packet::PacketKey key);
	bool Send(Packet::PacketKey key, Packet packet);

	void Subscribe(Packet::PacketKey key, OnReceivePacket onReceivePacket);
	void Unsubscribe(Packet::PacketKey key);
	void SubscribeAsync(Packet::PacketKey key, OnReceivePacket onReceivePacket);

	void SubscribeOnDisconnect(OnSocketDisconnect onSocketDisconect);
	
private:
	std::map<Packet::PacketKey, OnReceivePacket> _subscriptions;
	std::mutex _subscriptionsMutex;

	std::vector<OnSocketDisconnect> _onSocketDisconnectList;
	std::mutex _onSocketDisconnectMutex, packetManagementMutex;

	void ProcessPacket(Packet packet);
};

