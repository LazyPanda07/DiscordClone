#pragma once

#include "UDPSocket.hpp"

namespace web
{
	class UDPServerSocket : public UDPSocket
	{
	public:
		UDPServerSocket(uint16_t listenPort);
		
		void receiveData(const ReceiveCallback& callback) override;

		~UDPServerSocket() = default;
	};
}
