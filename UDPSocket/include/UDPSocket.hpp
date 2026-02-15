#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <array>
#include <span>
#include <stdexcept>
#include <format>
#include <cstring>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>

using socklen_t = int;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

#ifndef WINDOWS_STYLE_DEFINITION
#define WINDOWS_STYLE_DEFINITION

#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

using SOCKET = int;
using DWORD = uint32_t;

#endif // WINDOWS_STYLE_DEFINITION
#endif

namespace web
{
	class UDPSocket
	{
	public:
		using Buffer = std::array<char, 4096>;

	protected:
		SOCKET udpSocket;
		sockaddr_in address;

	protected:
		void initializeSockets();

	protected:
		UDPSocket(const sockaddr_in& address = {});

	public:
		template<typename T>
		int sendData(std::span<T> data, const UDPSocket* receiver = nullptr) const;

		int sendData(std::string_view data, const UDPSocket* receiver = nullptr) const;

		virtual void receiveData(const std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address, const UDPSocket& socket)>& callback) = 0;

		virtual ~UDPSocket();
	};
}

namespace web
{
	template<typename T>
	int UDPSocket::sendData(std::span<T> data, const UDPSocket* receiver) const
	{
		int result = sendto(udpSocket, reinterpret_cast<const char*>(data.data()), data.size_bytes(), 0, receiver ? reinterpret_cast<const sockaddr*>(&receiver->address) : reinterpret_cast<const sockaddr*>(&address), sizeof(address));

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
}
