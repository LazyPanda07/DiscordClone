#include "VoiceServer.hpp"

#include <iostream>
#include <format>

namespace voice
{
	std::tuple<std::string, uint16_t> VoiceServer::Client::getIpPort(const sockaddr_in& address)
	{
		std::string ip(INET_ADDRSTRLEN, '\0');

		inet_ntop
		(
			AF_INET,
			&address.sin_addr,
			ip.data(),
			INET_ADDRSTRLEN
		);

		return std::make_tuple(ip, ntohs(address.sin_port));
	}

	VoiceServer::Client::Client(const sockaddr_in& address) :
		address(address),
		socket(address),
		echo(false)
	{
		std::tie(ip, port) = VoiceServer::Client::getIpPort(address);
	}

	bool VoiceServer::Client::operator ==(const sockaddr_in& address) const noexcept
	{
		auto [otherIp, otherPort] = VoiceServer::Client::getIpPort(address);

		return ip == otherIp && port == otherPort;
	}

	VoiceServer::VoiceServer()
	{
		constexpr size_t predictedNumberOfClients = 4;

		clients.reserve(predictedNumberOfClients);
	}

	void VoiceServer::operator ()(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
	{
		if (size == web::UDPSocket::helloPacketSize && std::equal(data.begin(), data.begin() + size, web::UDPSocket::hello.begin()))
		{
			socket.sendData(web::UDPSocket::hello, address);

			return;
		}
		else if (size == web::UDPSocket::echoPacketSize && std::equal(data.begin(), data.begin() + size, web::UDPSocket::echo.begin()))
		{
			if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it != clients.end())
			{
				it->echo = !it->echo;
			}

			return;
		}
		else if (size == web::UDPSocket::pingPacketSize && std::equal(data.begin(), data.begin() + size, web::UDPSocket::ping.begin()))
		{
			socket.sendData(web::UDPSocket::ping, address);

			return;
		}
		else if (size == SOCKET_ERROR)
		{
			if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it != clients.end())
			{
				clients.erase(it);
			}

			auto [otherIp, otherPort] = VoiceServer::Client::getIpPort(address);

			std::cout << std::format("Wrong voice packet size: {}, from {}:{}", size, otherIp, otherPort) << std::endl;

			return;
		}

		const Client* currentClient = nullptr;

		if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it == clients.end())
		{
			currentClient = &clients.emplace_back(address);
		}
		else
		{
			currentClient = &*it;
		}

		for (const Client& client : clients)
		{
			if (currentClient != &client || currentClient->echo)
			{
				socket.sendData(std::span<const char>(data.data(), size), &client.socket);
			}
		}
	}
}
