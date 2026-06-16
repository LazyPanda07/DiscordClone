#pragma once

#include "UDPSocket.hpp"

namespace web
{
	class UDPClientSocket : public UDPSocket
	{
	private:
		sockaddr_in localAddress;

	private:
		void initLocalAddress();

	public:
		UDPClientSocket(std::string_view ip, uint16_t port);

		UDPClientSocket(const sockaddr_in& address);

		bool receiveData(const ReceiveCallback& callback, int32_t flags = 0) override;

		~UDPClientSocket() = default;
	};
}
