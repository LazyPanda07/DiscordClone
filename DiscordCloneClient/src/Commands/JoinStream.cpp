#include "Commands/JoinStream.hpp"

#include <chrono>

#include <PatternParser.h>
#include <HttpBuilder.h>
#include <HttpParser.h>
#include <JsonBuilder.h>
#include <UDPSocket.hpp>

#include "Utils.hpp"

constexpr std::string_view commandName = "join_stream";

template<>
struct utility::parsers::Converter<int32_t>
{
	constexpr void convert(std::string_view data, int32_t& result)
	{
		result = std::stoi(data.data());
	}
};

static std::string getServerIp(SOCKET socket);

namespace commands
{
	bool JoinStream::receiveHello(uint64_t id)
	{
		try
		{
			std::string data = videoStreamSocket->receiveData();

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

	bool JoinStream::run(std::istream& stream)
	{
		using namespace std::chrono_literals;

		constexpr size_t retries = 5;
		constexpr utility::parsers::PatternParser<int32_t, int32_t> parser("{}x{}");

		std::string line;
		int32_t width;
		int32_t height;

		std::string request;
		std::string response;
		json::JsonBuilder data;

		std::getline(stream, line);

		while (std::isspace(line.front()))
		{
			line.erase(line.begin());
		}

		parser.parse(line, width, height);

		data["userName"] = settings.userName;
		data["roomName"] = settings.roomName;
		data["roomPassword"] = settings.roomPassword;

		request = web::HttpBuilder()
			.postRequest()
			.parameters("room/video")
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
			std::string ip = getServerIp(controlStream->getNetwork().getClientSocket());

			videoStreamSocket = std::make_unique<wrappers::SocketWrapper<wrappers::SocketType::udp>>(ip, jsonData.get<uint16_t>("port"));

			videoStreamSocket->sendData(web::UDPSocket::constructHelloPacket(id));

			std::this_thread::sleep_for(50ms);

			for (size_t i = 0; i < retries; i++)
			{
				if (this->receiveHello(id))
				{
					streamThread = std::jthread(&utils::runViewStream, width, height, &videoStreamSocket);

					return true;
				}
			}
		}

		return true;
	}

	uint32_t JoinStream::getChecks() const
	{
		return checks::Check::socketStream;
	}

	JoinStream::JoinStream(const std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, std::jthread& streamThread, const client::Settings& settings, uint64_t id, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		controlStream(controlStream),
		videoStreamSocket(videoStreamSocket),
		settings(settings),
		streamThread(streamThread),
		id(id)
	{

	}

	std::string_view JoinStream::getHelpText() const
	{
		constexpr std::string_view helpText = "<width>x<height>";

		return helpText;
	}
}

std::string getServerIp(SOCKET socket)
{
	sockaddr_storage address{};
	std::string result(INET6_ADDRSTRLEN, '\0');

#ifdef __LINUX__
	socklen_t addressLength = sizeof(address);
#else
	int addressLength = sizeof(address);
#endif

	if (getpeername(socket, reinterpret_cast<sockaddr*>(&address), &addressLength) != 0)
	{
		return "";
	}

	int ret = getnameinfo
	(
		reinterpret_cast<sockaddr*>(&address),
		addressLength,
		result.data(),
		result.size(),
		NULL,
		0,
		NI_NUMERICHOST
	);

	return result;
}
