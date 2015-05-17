#include "Server.h"
#include <iostream>
#include <thread>
#include <WinSock2.h>

#include "common.h"
#include "log.h"
#include "TaskConsumerThread.h"

#pragma comment(lib, "ws2_32.lib")

namespace rte {

struct Server::Impl
{
	bool isConnected;
	SOCKET serverSock;
	SOCKET clientSock;

	int readTimeout;
	int writeTimeout;

	TaskConsumerThread<int> readThread;
	TaskConsumerThread<int> writeThread;

	Impl() :
		serverSock(INVALID_SOCKET), clientSock(INVALID_SOCKET),
		readTimeout(0), writeTimeout(0)
	{}
};

Server::Server()
{
	mpImpl = new Server::Impl();
}

Server::~Server()
{
	safeDelete(mpImpl);
}

bool Server::connect(const std::string& ip, int port)
{
	if (!mpImpl->isConnected)
	{
		WSADATA wsaData;
		int err = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (err != NO_ERROR)
		{
			log_error(err);
			return false;
		}
	}

	auto sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		log_error(WSAGetLastError());
		return false;
	}
	mpImpl->serverSock = sock;

	return true;
}

void Server::disconnect()
{
	if (mpImpl->isConnected)
	{
		WSACleanup();
		mpImpl->isConnected = false;
	}
}

void Server::setReadTimeout(int seconds)
{
}

size_t Server::read(void* buffer, size_t bufferLength)
{
	std::cout << strlen((char*)buffer) << bufferLength << std::endl;
	return 0;
}

void Server::readAsync(void* buffer, size_t bufferLength, std::function<void(size_t)> onReadComplete)
{
	if (onReadComplete != nullptr)
	{
		onReadComplete(bufferLength);
	}
}

void Server::setWriteTimeout(int seconds)
{
}

size_t Server::write(void* buffer, size_t bufferLength)
{
	return 0;
}

void Server::writeAsync(void* buffer, size_t bufferLength, std::function<void(size_t)> onWriteComplete)
{
	if (onWriteComplete != nullptr)
	{
		onWriteComplete(bufferLength);
	}
}

}// namespace rte
