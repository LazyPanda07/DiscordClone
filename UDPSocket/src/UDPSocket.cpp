#include "UDPSocket.hpp"

#include <stdexcept>
#include <format>
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

	UDPSocket::UDPSocket() :
		address()
	{
		this->initializeSockets();
	}

	int UDPSocket::sendData(std::string_view data, const UDPSocket* receiver) const
	{
		int result = sendto(udpSocket, data.data(), data.size(), 0, receiver ? reinterpret_cast<const sockaddr*>(&receiver->address) : reinterpret_cast<const sockaddr*>(&address), sizeof(address));

		if (result == SOCKET_ERROR)
		{
#ifdef _WIN32
			throw std::runtime_error(std::format("Can't send data: {}", WSAGetLastError()));
#else
			throw std::runtime_error(std::format("Can't send data: {}", strerror(errno)));
#endif
		}

		return result;
	}

	UDPSocket::~UDPSocket()
	{
		closesocket(udpSocket);
	}
}
