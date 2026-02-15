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

		void receiveData(const std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address, const UDPSocket& socket)>& callback) override;

		~UDPClientSocket() = default;
	};
}
