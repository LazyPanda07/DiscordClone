#include "UDPSocket.hpp"

#include <iostream>

namespace web
{
	std::string UDPSocket::constructHelloPacket(uint64_t id)
	{
		std::string result(UDPSocket::helloPacketSize, '\0');

		const char* ptr = reinterpret_cast<const char*>(&id);
		auto it = std::copy(UDPSocket::hello.begin(), UDPSocket::hello.end(), result.begin());

		std::copy(ptr, ptr + sizeof(id), it);

		return result;
	}

	void UDPSocket::initializeSockets()
	{
#ifndef __LINUX__
		WSADATA wsaData{};

		if (int result = WSAStartup(MAKEWORD(2, 2), &wsaData); result != 0)
		{
			throw std::runtime_error(std::format("WSAStartup failed: {}", result));
		}
#endif

		if (udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); udpSocket == INVALID_SOCKET)
		{
#ifdef __LINUX__
			throw std::runtime_error(std::format("Socket creation failed: {}", strerror(errno)));
#else
			throw std::runtime_error(std::format("Socket creation failed: {}", WSAGetLastError()));
#endif
		}

#ifdef __LINUX__
		int flags = fcntl(udpSocket, F_GETFL, 0);

		if (flags == -1)
		{
			std::cerr << "Can't F_GETFL on socket" << std::endl;

			return;
		}

		flags |= O_NONBLOCK;

		fcntl(udpSocket, F_SETFL, flags);
#else
		u_long blockingMode = 1;

		ioctlsocket(udpSocket, FIONBIO, &blockingMode);
#endif
	}

	UDPSocket::UDPSocket(const sockaddr_in& address) :
		address(address)
	{
		this->initializeSockets();
	}

	int UDPSocket::sendData(std::string_view data, const UDPSocket* receiver) const
	{
		return this->sendData(std::span<const char>(data), receiver);
	}

	int UDPSocket::sendData(std::string_view data, const sockaddr_in& address) const
	{
		return this->sendData(std::span<const char>(data), address);
	}

	void UDPSocket::close()
	{
		if (udpSocket == SOCKET_ERROR)
		{
			return;
		}

#ifdef __LINUX__
		shutdown(udpSocket, SHUT_RDWR);
#else
		shutdown(udpSocket, SD_BOTH);
#endif

		closesocket(udpSocket);

		udpSocket = SOCKET_ERROR;
	}

	SOCKET UDPSocket::getSocket() const
	{
		return udpSocket;
	}

	UDPSocket::~UDPSocket()
	{
		this->close();
	}
}
