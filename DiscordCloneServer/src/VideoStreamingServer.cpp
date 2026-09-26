#include "VideoStreamingServer.hpp"

static std::tuple<std::string, uint16_t> getIpPort(const sockaddr_in& address)
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

namespace server
{
	VideoStreamingServer::Client::Client(const sockaddr_in& address, uint64_t id) :
		address(address),
		socket(address),
		id(id)
	{

	}

	bool VideoStreamingServer::Client::operator ==(const sockaddr_in& address) const noexcept
	{
		auto [ip, port] = getIpPort(this->address);
		auto [otherIp, otherPort] = getIpPort(address);

		return ip == otherIp && port == otherPort;
	}

	void VideoStreamingServer::serve(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
	{
		if (size == SOCKET_ERROR)
		{
			std::erase(clients, address);

			if (clients.empty())
			{
				started = false;
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

			clients.emplace_back(address, id);

			return;
		}


	}

	VideoStreamingServer::VideoStreamingServer() :
		started(false)
	{

	}

	void VideoStreamingServer::start()
	{
		if (started)
		{
			return;
		}

		startHandle = std::async
		(
			std::launch::async,
			[this]()
			{
				started = true;

				web::UDPSocket::ReceiveCallback callback = std::bind(&VideoStreamingServer::serve, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

				while (started)
				{
					socket.receiveData(callback);
				}
			}
		);
	}
}
