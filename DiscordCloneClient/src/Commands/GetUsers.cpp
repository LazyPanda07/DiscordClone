#include "Commands/GetUsers.hpp"

#include <HttpBuilder.h>
#include <HttpParser.h>
#include <JsonArrayWrapper.h>

constexpr std::string_view commandName = "get_users";

namespace commands
{
	bool GetUsers::run(std::istream& stream)
	{
		std::string request;
		std::string response;

		request = web::HttpBuilder()
			.getRequest()
			.parameters("room")
			.build
			(
				{
					{ "roomName", settings.roomName },
					{ "roomPassword", settings.roomPassword }
				}
			);

		(*controlStream) << request;
		(*controlStream) >> response;

		web::HttpParser parser(response);

		if (parser.getResponseCode() >= 300)
		{
			std::cerr << parser.getBody() << std::endl;

			return false;
		}

		constexpr std::string_view color = "32";
		json::utility::JsonArrayWrapper jsonData(parser.getJson().getParsedData());
		std::vector<std::string> users = jsonData.as<std::string>();

		std::cout << "Users in room:" << std::endl;

		for (std::string_view user : users)
		{
			std::cout << std::format("\033[{}m{}\033[0m", color, user) << std::endl;
		}

		return true;
	}

	uint32_t GetUsers::getChecks() const
	{
		return checks::Check::socketStream &
			checks::Check::socketDatagram;
	}

	GetUsers::GetUsers(const std::unique_ptr<streams::IOSocketStream>& controlStream, const client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		controlStream(controlStream),
		settings(settings)
	{

	}
}
