#include "UDPClientSocket.hpp"

#include <stdexcept>
#include <format>
#include <array>

namespace web
{
	UDPClientSocket::UDPClientSocket(std::string_view ip, uint16_t port) :
		UDPSocket()
	{
		address.sin_family = AF_INET;
		address.sin_port = htons(port);

		inet_pton(AF_INET, ip.data(), &address.sin_addr);
	}

	void UDPClientSocket::receiveData(const std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address)>& callback)
	{
		std::array<char, 1024> data{};
		socklen_t size = sizeof(address);
		int result = recvfrom(udpSocket, data.data(), data.size(), 0, reinterpret_cast<sockaddr*>(&address), &size);

		if (result == SOCKET_ERROR)
		{
#ifdef _WIN32
			throw std::runtime_error(std::format("Can't receive data: {}", WSAGetLastError()));
#else
			throw std::runtime_error(std::format("Can't receive data: {}", strerror(errno)));
#endif
		}

		callback(data, size, address);
	}
}
