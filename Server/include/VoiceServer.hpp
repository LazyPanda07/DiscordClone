#pragma once

#include <unordered_map>
#include <tuple>
#include <chrono>
#include <mutex>

#include <UDPClientSocket.hpp>
#include <UDPServerSocket.hpp>

namespace voice
{
	class VoiceServer
	{
	private:
		struct Client
		{
		public:
			static std::tuple<std::string, uint16_t> getIpPort(const sockaddr_in& address);

		public:
			web::UDPClientSocket socket;
			sockaddr_in address;
			std::string ip;
			std::string userName;
			uint16_t port;
			bool echo;
			std::chrono::steady_clock::time_point aliveTimestamp;

		public:
			Client(const sockaddr_in& address);

			bool operator ==(const sockaddr_in& address) const noexcept;

			~Client() = default;
		};

	private:
		std::vector<Client> clients;
		web::UDPServerSocket socket;
		std::unordered_map<uint64_t, std::string> pendingClients;
		std::mutex pendingClientsMutex;
		bool started;

	private:
		void serve(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket);

	public:
		VoiceServer();

		void start();

		void addPendingClient(uint64_t id, std::string&& userName);

		uint16_t getPort() const;

		~VoiceServer();
	};
}
