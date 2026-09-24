#pragma once

#include "TCPSocket.hpp"

namespace web
{
	class TCPClientSocket : public TCPSocket
	{
	public:
		TCPClientSocket(std::string_view ip, uint16_t port);

		~TCPClientSocket() = default;
	};
}
