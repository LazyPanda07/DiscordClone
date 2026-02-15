#pragma once

#include "UDPSocket.hpp"

namespace web
{
	class UDPServerSocket : public UDPSocket
	{
	public:
		UDPServerSocket(uint16_t listenPort);
		
		void receiveData(const std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address, const UDPSocket& socket)>& callback) override;

		~UDPServerSocket() = default;
	};
}
