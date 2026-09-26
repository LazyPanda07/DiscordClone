#include "Commands/StartStream.hpp"

#include <PatternParser.h>
#include <JsonBuilder.h>
#include <HttpBuilder.h>
#include <HttpParser.h>

constexpr std::string_view commandName = "start_stream";

template<>
struct utility::parsers::Converter<uint32_t>
{
	constexpr void convert(std::string_view data, uint32_t& result)
	{
		result = std::stoi(data.data());
	}
};

template<>
struct utility::parsers::Converter<std::string>
{
	constexpr void convert(std::string_view data, std::string& result)
	{
		result = data;
	}
};

static std::string getServerIp(SOCKET socket);

namespace commands
{
	void StartStream::startStream(std::string_view userName, std::string_view roomName, std::string_view roomPassword, uint64_t id, uint32_t width, uint32_t height, bool showPreview, uint32_t frameRate)
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

			streamThread = std::jthread(&utils::runStream, width, height, showPreview, frameRate, &videoStreamSocket);
		}
	}

	bool StartStream::run(std::istream& stream)
	{
		constexpr utility::parsers::PatternParser<uint32_t, uint32_t, uint32_t, std::string> parser("{}x{} {} {}");

		std::string line;
		uint32_t width;
		uint32_t height;
		uint32_t frameRate;
		std::string showPreview;

		std::getline(stream, line);

		while (std::isspace(line.front()))
		{
			line.erase(line.begin());
		}

		parser.parse(line, width, height, frameRate, showPreview);

		this->startStream(settings.userName, settings.roomName, settings.roomPassword, id, width, height, showPreview == "y" || showPreview == "yes", frameRate);

		return true;
	}

	uint32_t StartStream::getChecks() const
	{
		return checks::Check::socketStream;
	}

	StartStream::StartStream(std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, client::Settings& settings, std::jthread& streamThread, uint64_t id, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		controlStream(controlStream),
		videoStreamSocket(videoStreamSocket),
		settings(settings),
		streamThread(streamThread),
		id(id)
	{

	}

	std::string_view StartStream::getHelpText() const
	{
		constexpr std::string_view helpText = "<width>x<height> <frame rate> <show preview y/n yes/no>";

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
