#include "NotificationsServer.hpp"

#include <chrono>

#include <TCPSocket.hpp>

namespace servers
{
	NotificationsServer::Client::Client(uint64_t id, SOCKET socket, std::function<void()>&& cleanup) :
		id(id),
		socket(socket),
		cleanup(std::move(cleanup))
	{

	}

	NotificationsServer::Client::Client(Client&& other) noexcept :
		Client(other.id, other.socket, std::move(other.cleanup))
	{

	}

	NotificationsServer::Client& NotificationsServer::Client::operator =(Client&& other) noexcept
	{
		const_cast<uint64_t&>(id) = other.id;
		socket = other.socket;
		cleanup = std::move(other.cleanup);

		return *this;
	}

	SOCKET NotificationsServer::Client::operator *() const
	{
		return socket;
	}

	NotificationsServer::Client::~Client()
	{
		if (static_cast<bool>(cleanup))
		{
			cleanup();
		}
	}

	void NotificationsServer::worker(std::stop_token stop)
	{
		using namespace std::chrono_literals;

		while (!stop.stop_requested())
		{
			{
				std::lock_guard<std::mutex> notificationsLock(notificationsMutex);

				if (notifications.size())
				{
					std::lock_guard<std::mutex> clientsLock(clientsMutex);

					for (auto&& [id, notification] : notifications)
					{
						for (const Client& client : clients)
						{
							if (client.id != id)
							{
								BaseTCPServer::sendBytes(*client, notification.data(), notification.size());
							}
						}
					}

					notifications.clear();
				}
			}

			std::this_thread::sleep_for(1s);
		}
	}

	void NotificationsServer::clientConnection(const std::string& ip, SOCKET clientSocket, sockaddr address, std::function<void()>& cleanup)
	{
		uint64_t id;

		BaseTCPServer::receiveBytes(clientSocket, &id, sizeof(id));

		std::lock_guard<std::mutex> lock(clientsMutex);

		Client client(id, clientSocket, std::move(cleanup));

		clients.emplace_back(std::move(client));
	}

	NotificationsServer::NotificationsServer(std::string_view ip) :
		BaseTCPServer("0", ip, 0, false)
	{
		handler = std::jthread(std::bind(&NotificationsServer::worker, this, std::placeholders::_1));
	}

	void NotificationsServer::pushNotification(uint64_t id, std::string_view notification)
	{
		std::string fullPacketNotification(notification);

		while (fullPacketNotification.size() != web::TCPSocket::notificationSize)
		{
			fullPacketNotification += '\0';
		}

		std::lock_guard<std::mutex> lock(notificationsMutex);

		notifications.emplace_back(id, notification);
	}

	void NotificationsServer::remove(uint64_t id)
	{
		std::scoped_lock<std::mutex, std::mutex> lock(clientsMutex, notificationsMutex);

		std::erase_if(clients, [id](const Client& client) { return client.id == id; });
		std::erase_if(notifications, [id](const auto& value) { return value.first == id; });
	}
}
