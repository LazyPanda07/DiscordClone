#include "UDPClientSocket.hpp"

#include <array>

namespace web
{
	void UDPClientSocket::initLocalAddress(int64_t timeout)
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

#ifdef __LINUX__
		timeval timeoutValue;

		timeoutValue.tv_sec = timeout / 1000;
		timeoutValue.tv_usec = (timeout - timeoutValue.tv_sec * 1000) * 1000;
#else
		DWORD timeoutValue = static_cast<DWORD>(timeout);
#endif

		if (setsockopt(udpSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			throw std::runtime_error("Can't set send timeout");
		}

		if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			throw std::runtime_error("Can't set send receive");
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

	void UDPClientSocket::receiveData(const ReceiveCallback& callback)
	{
		sockaddr_in server{};
		Buffer data{};
		socklen_t size = sizeof(server);
		int result = recvfrom(udpSocket, data.data(), data.size(), 0, reinterpret_cast<sockaddr*>(&server), &size);

		callback(data, result, server, *this);
	}
}
