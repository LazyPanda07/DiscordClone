#include "TCPClientSocket.hpp"

#include <format>

namespace web
{
	TCPClientSocket::TCPClientSocket(std::string_view ip, uint16_t port)
	{
		addrinfo* info = nullptr;
		addrinfo hints = {};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(ip.data(), std::to_string(port).data(), &hints, &info))
		{
			throw std::runtime_error(std::format("{}::{}: {}", __FILE__, __func__, __LINE__));
		}

		if (tcpSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol); tcpSocket == INVALID_SOCKET)
		{
			freeaddrinfo(info);

			throw std::runtime_error(std::format("{}::{}: {}", __FILE__, __func__, __LINE__));
		}

		if (connect(tcpSocket, info->ai_addr, static_cast<int>(info->ai_addrlen)) == SOCKET_ERROR)
		{
			freeaddrinfo(info);

			throw std::runtime_error(std::format("{}::{}: {}", __FILE__, __func__, __LINE__));
		}

		freeaddrinfo(info);
	}
}
