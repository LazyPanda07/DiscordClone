#pragma once

#include "TCPSocket.hpp"

namespace web
{
	class TCPServerSocket : public TCPSocket
	{
	public:
		TCPServerSocket(SOCKET socket);
		
		uint16_t getPort() const;

		~TCPServerSocket() = default;
	};
}
