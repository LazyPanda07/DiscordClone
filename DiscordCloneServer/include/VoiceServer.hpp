#pragma once

#include <unordered_map>
#include <tuple>
#include <chrono>
#include <mutex>
#include <future>

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
			uint64_t id;
			uint16_t port;
			bool echo;
			std::chrono::steady_clock::time_point aliveTimestamp;

		public:
			Client(const sockaddr_in& address, uint64_t id);

			bool operator ==(const sockaddr_in& address) const noexcept;

			~Client() = default;
		};

	private:
		std::vector<Client> clients;
		web::UDPServerSocket socket;
		std::unordered_map<uint64_t, std::pair<std::string, sockaddr_in>> pendingClients;
		std::vector<sockaddr_in> disconnectedClients;
		std::mutex pendingClientsMutex;
		std::mutex disconnectedClientsMutex;
		std::future<void> startFuture;
		bool started;

	private:
		void serve(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket);

	public:
		VoiceServer();

		void start();

		void addPendingClient(uint64_t id, std::string&& userName);
		
		void removeClient(uint64_t id);

		uint16_t getPort() const;

		std::vector<std::string> getClients() const;

		~VoiceServer();
	};
}
