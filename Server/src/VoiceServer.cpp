#include "VoiceServer.hpp"

namespace voice
{
	void VoiceServer::operator ()(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
	{
		IpData clientIp{};

		inet_ntop(AF_INET, &address.sin_addr, clientIp.data(), sizeof(clientIp));

		Client* currentClient = nullptr;

		if (auto it = std::ranges::find(clients, clientIp, &VoiceServer::Client::ip); it == clients.end())
		{
			currentClient = &clients.emplace_back(clientIp, address);
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
