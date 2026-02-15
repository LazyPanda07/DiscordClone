#pragma once

#include "UDPSocket.hpp"

namespace web
{
	class UDPClientSocket : public UDPSocket
	{
	public:
		UDPClientSocket(std::string_view ip, uint16_t port);

		void receiveData(const std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address)>& callback) override;

		~UDPClientSocket() = default;
	};
}
