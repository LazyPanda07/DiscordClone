#include "Checks/CheckSocket.hpp"

namespace checks
{
	CheckSocket::CheckSocket(const std::unique_ptr<wrappers::SocketWrapper>& socket) :
		socket(socket)
	{

	}

	bool CheckSocket::check() const
	{
		return static_cast<bool>(socket);
	}

	std::string_view CheckSocket::getErrorMessage() const
	{
		constexpr std::string_view errorMessage = "Not connected";

		return errorMessage;
	}

	Check::AvailableChecks CheckSocket::getCheck() const
	{
		return AvailableChecks::socketDatagram;
	}
}
