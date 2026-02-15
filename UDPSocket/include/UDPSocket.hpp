#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <array>

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
		using Buffer = std::array<char, 1024>;

	protected:
		SOCKET udpSocket;
		sockaddr_in address;

	protected:
		void initializeSockets();

	protected:
		UDPSocket();

	public:
		int sendData(std::string_view data, const UDPSocket* receiver = nullptr) const;

		virtual void receiveData(const std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address)>& callback) = 0;

		virtual ~UDPSocket();
	};
}
