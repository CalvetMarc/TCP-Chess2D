#include "SocketSelect.h"

bool SocketSelect::Wait()
{
	_mutex.lock();
	bool ready = wait();
	_mutex.unlock();

	return ready;
}

bool SocketSelect::IsReady(TcpListener& listener)
{
	_mutex.lock();
	bool ready = isReady(listener);
	_mutex.unlock();
	return ready;
}

bool SocketSelect::IsReady(TcpSocket& socket)
{
	_mutex.lock();
	bool ready = isReady(socket);
	_mutex.unlock();
	return ready;
}

void SocketSelect::Add(TcpListener& listener)
{
	_mutex.lock();
	add(listener);
	_mutex.unlock();

}

void SocketSelect::Add(TcpSocket& socket)
{
	_mutex.lock();
	add(socket);
	_mutex.unlock();
}

void SocketSelect::Remove(TcpListener& listener)
{
	_mutex.lock();
	remove(listener);
	_mutex.unlock();
}

void SocketSelect::Remove(TcpSocket& socket)
{
	_mutex.lock();
	remove(socket);
	_mutex.unlock();
}
