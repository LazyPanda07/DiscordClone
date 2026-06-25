#include "UDPClientSocket.hpp"

#include <iostream>
#include <array>

namespace web
{
	void UDPClientSocket::initLocalAddress()
	{
		localAddress = {};

		localAddress.sin_family = AF_INET;
		localAddress.sin_port = htons(0);
		localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(udpSocket, reinterpret_cast<sockaddr*>(&localAddress), sizeof(localAddress)) == SOCKET_ERROR)
		{
#ifdef __LINUX__
			throw std::runtime_error(std::format("Bind failed from client with: {} error", strerror(errno)));
#else
			throw std::runtime_error(std::format("Bind failed from client with: {} error", WSAGetLastError()));
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

	UDPClientSocket::UDPClientSocket(std::string_view ip, uint16_t port)
	{
		address.sin_family = AF_INET;
		address.sin_port = htons(port);

		inet_pton(AF_INET, ip.data(), &address.sin_addr);

		this->initLocalAddress();
	}

	UDPClientSocket::UDPClientSocket(const sockaddr_in& address) :
		UDPSocket(address)
	{
		this->initLocalAddress();
	}

	bool UDPClientSocket::receiveData(const ReceiveCallback& callback, int32_t flags)
	{
		sockaddr_in server{};
		Buffer data{};
		socklen_t size = sizeof(server);

		int result = recvfrom(udpSocket, data.data(), data.size(), flags, reinterpret_cast<sockaddr*>(&server), &size);

		callback(data, result, server, *this);

		return result != SOCKET_ERROR;
	}
}
