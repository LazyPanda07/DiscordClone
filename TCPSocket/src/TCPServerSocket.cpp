#include "TCPServerSocket.hpp"

namespace web
{
	TCPServerSocket::TCPServerSocket(SOCKET socket)
	{
		tcpSocket = socket;
	}
}
