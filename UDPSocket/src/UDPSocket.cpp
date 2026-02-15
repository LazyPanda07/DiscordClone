#include "UDPSocket.hpp"

#include <array>

namespace web
{
	void UDPSocket::initializeSockets()
	{
#if _WIN32
		WSADATA wsaData;

		if (int result = WSAStartup(MAKEWORD(2, 2), &wsaData); result != 0)
		{
			throw std::runtime_error(std::format("WSAStartup failed: {}", result));
		}
#endif

		if (udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); udpSocket == INVALID_SOCKET)
		{
#if _WIN32
			throw std::runtime_error(std::format("Socket creation failed: {}", WSAGetLastError()));
#else
			throw std::runtime_error(std::format("Socket creation failed: {}", strerror(errno)));
#endif
		}
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

	UDPSocket::~UDPSocket()
	{
		closesocket(udpSocket);
	}
}
