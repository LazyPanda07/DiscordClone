#include "TCPSocket.hpp"

namespace web
{
	TCPSocket::TCPSocket() :
		tcpSocket(INVALID_SOCKET)
	{
#ifndef __LINUX__
		WSADATA wsaData{};

		if (int result = WSAStartup(MAKEWORD(2, 2), &wsaData); result != 0)
		{
			throw std::runtime_error(std::format("WSAStartup failed: {}", result));
		}
#endif
	}

	void TCPSocket::close()
	{
		if (tcpSocket == SOCKET_ERROR)
		{
			return;
		}

#ifdef __LINUX__
		shutdown(tcpSocket, SHUT_RDWR);
#else
		shutdown(tcpSocket, SD_BOTH);
#endif

		closesocket(tcpSocket);

		tcpSocket = SOCKET_ERROR;
	}

	SOCKET TCPSocket::getSocket() const
	{
		return tcpSocket;
	}

	TCPSocket::~TCPSocket()
	{
		this->close();
	}
}
