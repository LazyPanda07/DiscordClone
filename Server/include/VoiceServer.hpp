#pragma once

#include <array>
#include <unordered_map>

#include <UDPClientSocket.hpp>

namespace voice
{
	class VoiceServer
	{
	private:
		struct Client
		{
		public:
			std::string uuid;
			web::UDPClientSocket socket;

		public:
			Client(std::string_view uuid, const sockaddr_in& address);

			~Client() = default;
		};

	private:
		std::vector<Client> clients;

	public:
		VoiceServer() = default;

		void operator ()(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket);

		~VoiceServer() = default;
	};
}
