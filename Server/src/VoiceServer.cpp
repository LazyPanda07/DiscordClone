#include "VoiceServer.hpp"

namespace voice
{
	VoiceServer::Client::Client(std::string_view uuid, const sockaddr_in& address) :
		uuid(uuid),
		socket(address)
	{

	}

	void VoiceServer::operator ()(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
	{
		std::string_view uuid(data.data(), web::UDPSocket::uuidSize);
		Client* currentClient = nullptr;

		if (auto it = std::ranges::find(clients, uuid, &VoiceServer::Client::uuid); it == clients.end())
		{
			currentClient = &clients.emplace_back(uuid, address);
		}
		else
		{
			currentClient = &*it;
		}

		for (Client& client : clients)
		{
			if (currentClient != &client)
			{
				socket.sendData(std::span<const char>(data.data() + web::UDPSocket::uuidSize, size), &client.socket);
			}
		}
	}
}
