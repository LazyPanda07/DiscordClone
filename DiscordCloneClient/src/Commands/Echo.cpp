#include "Commands/Echo.hpp"

#include <UDPSocket.hpp>

constexpr std::string_view commandName = "echo";

namespace commands
{
	bool Echo::run(std::istream& stream)
	{
		socket->sendData(web::UDPSocket::echo);

		return true;
	}

	uint32_t Echo::getChecks() const
	{
		return checks::Check::AvailableChecks::socketDatagram;
	}

	Echo::Echo(const std::unique_ptr<wrappers::SocketWrapper>& socket, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		socket(socket)
	{

	}
}
