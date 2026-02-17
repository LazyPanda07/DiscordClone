#include "VoiceServer.hpp"

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
		socket(address)
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
		if (size != web::UDPSocket::voicePacketSize)
		{
			if (size == SOCKET_ERROR)
			{
				if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it == clients.end())
				{
					clients.erase(it);
				}
			}

			return;
		}

		Client* currentClient = nullptr;

		if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it == clients.end())
		{
			currentClient = &clients.emplace_back(address);
		}
		else
		{
			currentClient = &*it;
		}

		for (Client& client : clients)
		{
			if (currentClient != &client)
			{
				socket.sendData(std::span<const char>(data.data(), size), &client.socket);
			}
		}
	}
}
