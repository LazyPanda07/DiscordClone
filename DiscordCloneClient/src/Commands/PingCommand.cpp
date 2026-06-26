#include "Commands/PingCommand.hpp"

#include <chrono>

#include <HttpBuilder.h>
#include <HttpParser.h>

constexpr std::string_view commandName = "ping";

namespace commands
{
	bool PingCommand::run(std::istream& stream)
	{
		std::string request = web::HttpBuilder()
			.getRequest()
			.parameters("ping")
			.build("ping");
		std::string response;

		auto start = std::chrono::high_resolution_clock::now();

		(*controlStream) << request;
		(*controlStream) >> response;

		auto end = std::chrono::high_resolution_clock::now();

		if (web::HttpParser(response).getResponseCode() == 200)
		{
			std::cout << std::format("Ping is: ~{} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) << std::endl;

			return true;
		}

		std::cerr << "Can't get ping" << std::endl;
		std::cerr << response << std::endl;

		return false;
	}

	uint32_t PingCommand::getChecks() const
	{
		return checks::Check::AvailableChecks::socketStream;
	}

	PingCommand::PingCommand(const std::unique_ptr<streams::IOSocketStream>& controlStream, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		controlStream(controlStream)
	{

	}
}
