#pragma once

#include <BaseTCPServer.h>

#include <functional>
#include <vector>
#include <string>
#include <mutex>

namespace server
{
	class NotificationsServer : public web::BaseTCPServer
	{
	private:
		class Client
		{
		public:
			const uint64_t id;

		private:
			SOCKET socket;
			std::function<void()> cleanup;

		public:
			Client(uint64_t id, SOCKET socket, std::function<void()>&& cleanup);

			Client(const Client&) = delete;

			Client(Client&& other) noexcept;

			Client& operator =(const Client&) = delete;

			Client& operator =(Client&& other) noexcept;

			SOCKET operator *() const;

			~Client();
		};

	private:
		std::vector<Client> clients;
		std::vector<std::pair<uint64_t, std::string>> notifications;
		std::mutex clientsMutex;
		std::mutex notificationsMutex;
		std::future<void> handler;

	private:
		void worker();

	private:
		void clientConnection(const std::string& ip, SOCKET clientSocket, sockaddr address, std::function<void()>& cleanup) override;

	public:
		NotificationsServer(std::string_view ip);

		void pushNotification(uint64_t id, std::string_view notification);

		void remove(uint64_t id);

		~NotificationsServer() = default;
	};
}
