#include "UDPServerSocket.hpp"

#include <array>
#include <chrono>

namespace web
{
	UDPServerSocket::UDPServerSocket() :
		UDPSocket()
	{
		using namespace std::chrono_literals;

		int addressSize = sizeof(address);

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
		int64_t timeout = std::chrono::duration_cast<std::chrono::milliseconds>(60s).count();

#ifdef __LINUX__
		timeval timeoutValue;

		timeoutValue.tv_sec = timeout / 1000;
		timeoutValue.tv_usec = (timeout - timeoutValue.tv_sec * 1000) * 1000;
#else
		DWORD timeoutValue = static_cast<DWORD>(timeout);
#endif

		if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
#ifdef __LINUX__
			throw std::runtime_error(std::format("recvfrom timeout setting failed from server with: {} error", strerror(errno)));
#else
			throw std::runtime_error(std::format("recvfrom timeout setting failed from server with: {} error", WSAGetLastError()));
#endif
		}
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
