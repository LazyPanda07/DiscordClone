#include "Commands/StopStream.hpp"

#include <JsonBuilder.h>
#include <HttpBuilder.h>
#include <HttpParser.h>

constexpr std::string_view commandName = "stop_stream";

namespace commands
{
	bool StopStream::run(std::istream& stream)
	{
		streamThread.request_stop();

		std::string request;
		std::string response;
		json::JsonBuilder data;

		data["userName"] = settings.userName;
		data["roomName"] = settings.roomName;
		data["roomPassword"] = settings.roomPassword;

		request = web::HttpBuilder()
			.deleteRequest()
			.parameters("room/video")
			.build(data);

		(*controlStream) << request;

		videoStreamSocket.reset();

		(*controlStream) >> response;

		return true;
	}

	uint32_t StopStream::getChecks() const
	{
		return checks::Check::socketStream;
	}

	StopStream::StopStream(std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, std::jthread& streamThread, const client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		controlStream(controlStream),
		videoStreamSocket(videoStreamSocket),
		streamThread(streamThread),
		settings(settings)
	{

	}
}
