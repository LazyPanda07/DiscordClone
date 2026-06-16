#include "UDPClientSocket.hpp"

#include <array>

namespace web
{
	void UDPClientSocket::initLocalAddress()
	{
		localAddress = {};

		localAddress.sin_family = AF_INET;
		localAddress.sin_addr.s_addr = INADDR_ANY;
		localAddress.sin_port = htons(0);

		if (bind(udpSocket, reinterpret_cast<sockaddr*>(&localAddress), sizeof(localAddress)) == SOCKET_ERROR)
		{
#ifdef __LINUX__
			throw std::runtime_error(std::format("Bind failed: {}", strerror(errno)));
#else
			throw std::runtime_error(std::format("Bind failed: {}", WSAGetLastError()));
#endif
		}
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

#ifndef __LINUX__
		u_long mode = 1;
		bool customNonBlocking = flags == UDPSocket::customNonBlockingFlag;
		
		if (customNonBlocking)
		{
			ioctlsocket(udpSocket, FIONBIO, &mode);

			flags = 0;
		}

		int result = recvfrom(udpSocket, data.data(), data.size(), flags, reinterpret_cast<sockaddr*>(&server), &size);

		if (customNonBlocking)
		{
			mode = 0;

			ioctlsocket(udpSocket, FIONBIO, &mode);
		}
#else
		int result = recvfrom(udpSocket, data.data(), data.size(), flags, reinterpret_cast<sockaddr*>(&server), &size);
#endif

		callback(data, result, server, *this);

		return result != SOCKET_ERROR;
	}
}
