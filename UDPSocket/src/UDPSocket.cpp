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
			// TODO: Linux exception
#endif
		}
	}

	UDPSocket::UDPSocket(std::string_view ip, uint16_t port) :
		address()
	{
		this->initializeSockets();

		address.sin_family = AF_INET;
		address.sin_port = htons(port);

		inet_pton(AF_INET, ip.data(), &address.sin_addr);
	}

	UDPSocket::UDPSocket(uint16_t listenPort) :
		address()
	{
		this->initializeSockets();

		address.sin_family = AF_INET;
		address.sin_port = htons(listenPort);
		address.sin_addr.s_addr = INADDR_ANY;

		if (bind(udpSocket, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
		{
#ifdef _WIN32
			throw std::runtime_error(std::format("Bind failed: {}", WSAGetLastError()));
#else
			// TODO: Linux exception
#endif
		}
	}

	int UDPSocket::sendData(std::string_view data) const
	{
		int result = sendto(udpSocket, data.data(), data.size(), 0, reinterpret_cast<const sockaddr*>(&address), sizeof(address));

		if (result == SOCKET_ERROR)
		{
#ifdef _WIN32
			throw std::runtime_error(std::format("Can't send data: {}", WSAGetLastError()));
#else
			// TODO: Linux exception
#endif
		}

		return result;
	}

	std::string UDPSocket::receiveData()
	{
		std::array<char, 1024> data{};
		socklen_t size = sizeof(address);
		int result = recvfrom(udpSocket, data.data(), data.size(), 0, reinterpret_cast<sockaddr*>(&address), &size);

		if (result == SOCKET_ERROR)
		{
			return "";
		}

		return std::string(data.begin(), data.begin() + result);
	}

	UDPSocket::~UDPSocket()
	{
		closesocket(udpSocket);
	}
}
