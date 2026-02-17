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
#ifdef _WIN32
			throw std::runtime_error(std::format("Bind failed: {}", WSAGetLastError()));
#else
			throw std::runtime_error(std::format("Bind failed: {}", strerror(errno)));
#endif
		}
	}

	UDPClientSocket::UDPClientSocket(std::string_view ip, uint16_t port) :
		UDPSocket()
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
