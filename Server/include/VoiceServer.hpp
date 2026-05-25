#pragma once

#include <unordered_map>
#include <tuple>

#include <UDPClientSocket.hpp>

namespace voice
{
	class VoiceServer
	{
	private:
		struct Client
		{
		private:
			static std::tuple<std::string, uint16_t> getIpPort(const sockaddr_in& address);

		public:
			web::UDPClientSocket socket;
			sockaddr_in address;
			std::string ip;
			uint16_t port;
			bool echo;

		public:
			Client(const sockaddr_in& address);

			bool operator ==(const sockaddr_in& address) const noexcept;

			~Client() = default;
		};

	private:
		std::vector<Client> clients;

	public:
		VoiceServer();

		void operator ()(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket);

		~VoiceServer() = default;
	};
}
