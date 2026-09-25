#include "Commands/StartStream.hpp"

#include <PatternParser.h>
#include <JsonBuilder.h>
#include <HttpBuilder.h>
#include <HttpParser.h>

constexpr std::string_view commandName = "start_stream";

template<>
struct utility::parsers::Converter<uint16_t>
{
	constexpr void convert(std::string_view data, uint16_t& result)
	{
		result = std::stoi(data.data());
	}
};

static std::string getServerIp(SOCKET socket);

namespace commands
{
	void StartStream::startStream(std::string_view userName, std::string_view roomName, std::string_view roomPassword, uint64_t id)
	{
		std::string request;
		std::string response;
		json::JsonBuilder data;

		data["userName"] = userName;
		data["roomName"] = roomName;
		data["roomPassword"] = roomPassword;
		data["id"] = id;

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
		}
	}

	bool StartStream::run(std::istream& stream)
	{
		constexpr utility::parsers::PatternParser<uint16_t, uint16_t, uint16_t> parser("{}x{} {}");

		std::string line;
		uint16_t width;
		uint16_t height;
		uint16_t frameRate;

		std::getline(stream, line);

		while (std::isspace(line.front()))
		{
			line.erase(line.begin());
		}

		parser.parse(line, width, height, frameRate);

		this->startStream(settings.userName, settings.roomName, settings.roomPassword, id);

		return true;
	}

	uint32_t StartStream::getChecks() const
	{
		return checks::Check::socketStream;
	}

	StartStream::StartStream(std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, const client::Settings& settings, uint64_t id, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		controlStream(controlStream),
		videoStreamSocket(videoStreamSocket),
		settings(settings),
		id(id)
	{

	}

	std::string_view StartStream::getHelpText() const
	{
		constexpr std::string_view helpText = "<width>x<height> <frame rate>";

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
