#include "UDPServerSocket.hpp"

#include <array>
#include <chrono>

namespace web
{
	UDPServerSocket::UDPServerSocket() :
		UDPSocket()
	{
		using namespace std::chrono_literals;

		socklen_t addressSize = sizeof(address);

		address.sin_family = AF_INET;
		address.sin_port = htons(0);
		address.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(udpSocket, reinterpret_cast<sockaddr*>(&address), addressSize) == SOCKET_ERROR)
		{
#ifdef __LINUX__
			throw std::runtime_error(std::format("Bind failed from server with: {} error", strerror(errno)));
#else
			throw std::runtime_error(std::format("Bind failed from server with: {} error", WSAGetLastError()));
#endif
		}

		getsockname(udpSocket, reinterpret_cast<sockaddr*>(&address), &addressSize);
	}

	uint16_t UDPServerSocket::getPort() const
	{
		return ntohs(address.sin_port);
	}

	bool UDPServerSocket::receiveData(const ReceiveCallback& callback, int32_t flags)
	{
		sockaddr_in client{};
		Buffer data{};
		socklen_t size = sizeof(client);
		int result = recvfrom(udpSocket, data.data(), data.size(), flags, reinterpret_cast<sockaddr*>(&client), &size);

		callback(data, result, client, *this);

		return result != SOCKET_ERROR;
	}
}
