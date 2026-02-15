#pragma once

#include <array>
#include <unordered_map>

#include <UDPClientSocket.hpp>

namespace voice
{
	class VoiceServer
	{
	public:
		using IpData = std::array<char, INET_ADDRSTRLEN>;

	private:
		struct Client
		{
			IpData ip;
			web::UDPClientSocket socket;
		};

	private:
		std::vector<Client> clients;

	public:
		VoiceServer() = default;

		void operator ()(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket);

		~VoiceServer() = default;
	};
}
