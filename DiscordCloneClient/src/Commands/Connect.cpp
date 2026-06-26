#include "Commands/Connect.hpp"

#include <iostream>
#include <chrono>
#include <thread>

#include <Http/HttpNetwork.h>
#include <UDPSocket.hpp>
#include <HttpBuilder.h>
#include <HttpParser.h>
#include <PatternParser.h>

constexpr std::string_view commandName = "connect";

template<>
struct utility::parsers::Converter<std::string>
{
	constexpr void convert(std::string_view data, std::string& result)
	{
		result = data;
	}
};

template<>
struct utility::parsers::Converter<uint16_t>
{
	constexpr void convert(std::string_view data, uint16_t& result)
	{
		result = std::stoi(data.data());
	}
};

namespace commands
{
	bool Connect::connect(std::string_view ip, uint16_t port, std::string_view userName, std::string_view roomName, std::string_view roomPassword, uint64_t& id)
	{
		try
		{
			if (!controlStream)
			{
				using namespace std::chrono_literals;

				controlStream = std::make_unique<streams::IOSocketStream>(streams::IOSocketStream::createStream<web::http::HttpNetwork>(ip, std::to_string(port), 5s));
			}

			std::string request;
			std::string response;
			json::JsonBuilder data;
			uint16_t udpPort = 0;

			data["userName"] = userName;
			data["roomName"] = roomName;
			data["roomPassword"] = roomPassword;

			request = web::HttpBuilder()
				.getRequest()
				.parameters("room")
				.build(data);

			(*controlStream) << request;
			(*controlStream) >> response;

			// TODO: print error and disable reconnecting
			if (web::HttpParser parser(response); parser.getResponseCode() >= 300)
			{
				std::cerr << parser.getBody() << std::endl;

				throw std::runtime_error(parser.getBody());
			}
			else
			{
				const json::JsonParser& jsonData = parser.getJson();

				id = jsonData.get<uint64_t>("id");
				udpPort = jsonData.get<uint16_t>("port");
			}

			socket = std::make_unique<wrappers::SocketWrapper>(ip, udpPort);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool Connect::sendHello(uint64_t id)
	{
		try
		{
			socket->sendData(web::UDPSocket::constructHelloPacket(id));
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool Connect::receiveHello(uint64_t id)
	{
		try
		{
			std::string data = socket->receiveData();

			if (data.size() != web::UDPSocket::helloPacketSize)
			{
				return false;
			}

			uint64_t idFromServer = 0;
			char* ptr = reinterpret_cast<char*>(&idFromServer);

			for (size_t i = 0; i < sizeof(idFromServer); i++)
			{
				*ptr = data[web::UDPSocket::helloMessageSize + i];

				ptr++;
			}

			return id == idFromServer;
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
		constexpr utility::parsers::PatternParser<std::string, uint16_t, std::string, std::string, std::string> parser("{}:{} {} {} {}");

		std::string ip;
		uint16_t port;
		std::string userName;
		std::string roomName;
		std::string roomPassword;
		std::string line;
		uint64_t id;
		bool connected = false;

		std::getline(stream, line);

		while (std::isspace(line.front()))
		{
			line.erase(line.begin());
		}

		parser.parse(line, ip, port, userName, roomName, roomPassword);

		for (size_t i = 0; i < retries; i++)
		{
			std::cout << std::format("Connecting to: {}:{}...", ip, port) << std::endl;

			if (this->connect(ip, port, userName, roomName, roomPassword, id))
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
			if (this->sendHello(id))
			{
				break;
			}
		}

		std::this_thread::sleep_for(50ms);

		for (size_t i = 0; i < retries; i++)
		{
			if (this->receiveHello(id))
			{
				std::cout << "Establish voice stream" << std::endl;

				onSuccess();

				settings.modifySettings
				(
					[&ip, port, &userName, &roomName, &roomPassword](client::Settings& self)
					{
						self.reconnectIp = std::move(ip);
						self.reconnectPort = port;
						self.userName = std::move(userName);
						self.roomName = std::move(roomName);
						self.roomPassword = std::move(roomPassword);
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
		constexpr std::string_view helpText = "<ip>:<port> <user_name> <room_name> <room_password>";

		return helpText;
	}
}
