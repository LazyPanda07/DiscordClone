#pragma once

#include <string>
#include <cstdint>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

#ifndef WINDOWS_STYLE_DEFINITION
#define WINDOWS_STYLE_DEFINITION

#define closesocket close
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define DWORD uint32_t

#endif // WINDOWS_STYLE_DEFINITION
#endif

namespace web
{
	class UDPSocket
	{
	private:
		SOCKET udpSocket;
		sockaddr_in address;

	private:
		void initializeSockets();

	public:
		UDPSocket(std::string_view ip, uint16_t port);

		UDPSocket(uint16_t listenPort);

		int sendData(std::string_view data) const;

		std::string receiveData();

		~UDPSocket();
	};
}
