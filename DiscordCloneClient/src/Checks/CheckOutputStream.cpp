#include "Checks/CheckOutputStream.hpp"

namespace checks
{
	CheckOutputStream::CheckOutputStream(const std::unique_ptr<wrappers::OutputVoice>& input) :
		input(input)
	{

	}

	bool CheckOutputStream::check() const
	{
		return static_cast<bool>(input);
	}

	std::string_view CheckOutputStream::getErrorMessage() const
	{
		constexpr std::string_view errorMessage = "Output not initialized";

		return errorMessage;
	}

	Check::AvailableChecks CheckOutputStream::getCheck() const
	{
		return AvailableChecks::outputStream;
	}
}
