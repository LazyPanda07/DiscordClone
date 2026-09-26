#pragma once

#include <future>

#include <UDPServerSocket.hpp>
#include <UDPClientSocket.hpp>

namespace server
{
	class VideoStreamingServer
	{
	private:
		struct Client
		{
		public:
			web::UDPClientSocket socket;
			sockaddr_in address;
			uint64_t id;
			uint16_t port;

		public:
			Client(const sockaddr_in& address, uint64_t id);

			bool operator ==(const sockaddr_in& address) const noexcept;

			~Client() = default;
		};

	private:
		web::UDPServerSocket socket;
		std::vector<Client> clients;
		std::future<void> startHandle;
		bool started;

	private:
		void serve(const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket);

	public:
		VideoStreamingServer();

		void start();

		~VideoStreamingServer() = default;
	};
}
