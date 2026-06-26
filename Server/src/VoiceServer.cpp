#include "VoiceServer.hpp"

#include <iostream>

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

	void VoiceServer::serve(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
	{
		auto timestamp = std::chrono::steady_clock::now();

		if (size == SOCKET_ERROR)
		{
			constexpr int32_t timeoutValue = 30;

			if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it != clients.end())
			{
				clients.erase(it);
			}

			for (int32_t i = 0; i < static_cast<int32_t>(clients.size()); i++)
			{
				if (std::chrono::duration_cast<std::chrono::seconds>(clients[i].aliveTimestamp.time_since_epoch()).count() >= timeoutValue)
				{
					clients.erase(clients.begin() + i);

					i--;
				}
			}

			if (clients.empty())
			{
				started = false;
			}

			return;
		}
		else if (size == web::UDPSocket::alivePacketSize && std::equal(data.begin(), data.begin() + size, web::UDPSocket::alive.begin()))
		{
			if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it != clients.end())
			{
				it->aliveTimestamp = timestamp;
			}

			return;
		}
		else if (size == web::UDPSocket::helloPacketSize && std::equal(data.begin(), data.begin() + web::UDPSocket::helloMessageSize, web::UDPSocket::hello.begin()))
		{
			uint64_t id = 0;
			char* ptr = reinterpret_cast<char*>(&id);

			for (size_t i = 0; i < sizeof(id); i++)
			{
				*ptr = data[web::UDPSocket::helloMessageSize + i];

				ptr++;
			}

			socket.sendData(web::UDPSocket::constructHelloPacket(id), address);

			Client& client = clients.emplace_back(address);

			std::lock_guard<std::mutex> lock(pendingClientsMutex);

			if (auto it = pendingClients.find(id); it != pendingClients.end())
			{
				client.userName = std::move(pendingClients.extract(it).mapped());
			}
			else
			{
				std::cerr << "Can't find user with id: " << id << std::endl;
			}

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

		Client* currentClient = nullptr;

		if (auto it = std::ranges::find_if(clients, [&address](const Client& client) { return client == address; }); it == clients.end())
		{
			return;
		}
		else
		{
			currentClient = &*it;
		}

		currentClient->aliveTimestamp = timestamp;

		for (const Client& client : clients)
		{
			if (currentClient != &client || currentClient->echo)
			{
				socket.sendData(std::span<const char>(data.data(), size), &client.socket);
			}
		}
	}

	VoiceServer::VoiceServer() :
		started(false)
	{
		constexpr size_t predictedNumberOfClients = 4;

		clients.reserve(predictedNumberOfClients);
	}

	void VoiceServer::start()
	{
		if (started)
		{
			return;
		}

		started = true;

		web::UDPSocket::ReceiveCallback callback = std::bind(&VoiceServer::serve, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		while (started)
		{
			socket.receiveData(callback);
		}
	}

	void VoiceServer::addPendingClient(uint64_t id, std::string&& userName)
	{
		std::lock_guard<std::mutex> lock(pendingClientsMutex);

		pendingClients.try_emplace(id, std::move(userName));
	}

	uint16_t VoiceServer::getPort() const
	{
		return socket.getPort();
	}

	std::vector<std::string> VoiceServer::getClients() const
	{
		std::vector<std::string> result;

		result.reserve(clients.size());

		for (const Client& client : clients)
		{
			result.push_back(client.userName);
		}

		return result;
	}

	VoiceServer::~VoiceServer()
	{
		started = false;
	}
}
