#include "UDPServerSocket.hpp"

#include <array>

namespace web
{
	UDPServerSocket::UDPServerSocket(uint16_t listenPort) :
		UDPSocket()
	{
		address.sin_family = AF_INET;
		address.sin_port = htons(listenPort);
		address.sin_addr.s_addr = INADDR_ANY;

		if (bind(udpSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR)
		{
#ifdef __LINUX__
			throw std::runtime_error(std::format("Bind failed: {}", strerror(errno)));
#else
			throw std::runtime_error(std::format("Bind failed: {}", WSAGetLastError()));
#endif
		}
	}

	void UDPServerSocket::receiveData(const ReceiveCallback& callback)
	{
		sockaddr_in client{};
		Buffer data{};
		socklen_t size = sizeof(client);
		int result = recvfrom(udpSocket, data.data(), data.size(), 0, reinterpret_cast<sockaddr*>(&client), &size);

		callback(data, result, client, *this);
	}
}
