#include "Checks/CheckStream.hpp"

namespace checks
{
	CheckStream::CheckStream(const std::unique_ptr<streams::IOSocketStream>& stream) :
		stream(stream)
	{

	}

	bool CheckStream::check() const
	{
		return static_cast<bool>(stream);
	}

	std::string_view CheckStream::getErrorMessage() const
	{
		constexpr std::string_view errorMessage = "Not connected stream";

		return errorMessage;
	}

	Check::AvailableChecks CheckStream::getCheck() const
	{
		return AvailableChecks::socketStream;
	}
}
