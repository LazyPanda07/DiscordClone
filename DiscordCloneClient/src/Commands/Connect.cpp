#include "Commands/Connect.hpp"

#include <iostream>
#include <chrono>
#include <thread>

#include <Http/HttpNetwork.h>
#include <UDPSocket.hpp>

constexpr std::string_view commandName = "connect";

namespace commands
{
	bool Connect::connect(std::string_view ip, int16_t port)
	{
		try
		{
			if (!controlStream)
			{
				using namespace std::chrono_literals;

				controlStream = std::make_unique<streams::IOSocketStream>(streams::IOSocketStream::createStream<web::http::HttpNetwork>(ip, std::to_string(port), 5s));
			}

			socket = std::make_unique<wrappers::SocketWrapper>(ip, port + 1);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool Connect::sendHello()
	{
		try
		{
			socket->sendData(web::UDPSocket::hello);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool Connect::receiveHello()
	{
		try
		{
			return socket->receiveData() == web::UDPSocket::hello;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	bool Connect::run(std::istream& stream)
	{
		using namespace std::chrono_literals;

		constexpr size_t retries = 5;

		std::string ip;
		int16_t port;
		bool connected = false;

		std::getline(stream, ip, ':');

		while (std::isspace(static_cast<uint8_t>(ip.front())))
		{
			ip.erase(ip.begin());
		}

		stream >> port;

		for (size_t i = 0; i < retries; i++)
		{
			std::cout << std::format("Connecting to: {}:{}...", ip, port) << std::endl;

			if (this->connect(ip, port))
			{
				std::cout << std::format("Connected to: {}:{}", ip, port) << std::endl;

				connected = true;

				break;
			}
		}

		if (!connected)
		{
			std::cout << std::format("Can't connect to: {}:{}", ip, port) << std::endl;

			return false;
		}

		std::cout << "Waiting for server to response..." << std::endl;

		for (size_t i = 0; i < retries; i++)
		{
			if (this->sendHello())
			{
				break;
			}
		}

		std::this_thread::sleep_for(50ms);

		for (size_t i = 0; i < retries; i++)
		{
			if (this->receiveHello())
			{
				std::cout << "Establish voice stream" << std::endl;

				onSuccess();

				settings.modifySettings
				(
					[&ip, port](client::Settings& self)
					{
						self.reconnectIp = std::move(ip);
						self.reconnectPort = port;
					}
				);

				return true;
			}

			std::this_thread::sleep_for(1s);

			std::cout << std::format("Establishing voice stream with: {}:{}...", ip, port) << std::endl;
		}

		std::cout << std::format("Failed to establish voice stream with: {}:{}", ip, port) << std::endl;

		return false;
	}

	uint32_t Connect::getChecks() const
	{
		return NULL;
	}

	Connect::Connect(std::unique_ptr<wrappers::SocketWrapper>& socket, std::unique_ptr<streams::IOSocketStream>& controlStream, client::Settings& settings, const std::function<void()>& onSuccess, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		socket(socket),
		controlStream(controlStream),
		settings(settings),
		onSuccess(onSuccess)
	{

	}

	std::string_view Connect::getHelpText() const
	{
		constexpr std::string_view helpText = "<ip:port>";

		return helpText;
	}
}
