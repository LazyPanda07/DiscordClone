#pragma once

#include "UDPSocket.hpp"

namespace web
{
	class UDPServerSocket : public UDPSocket
	{
	public:
		UDPServerSocket(uint16_t listenPort);
		
		void receiveData(const ReceiveCallback& callback, int32_t flags = 0) override;

		~UDPServerSocket() = default;
	};
}
