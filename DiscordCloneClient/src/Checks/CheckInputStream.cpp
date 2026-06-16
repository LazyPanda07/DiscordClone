#include "Checks/CheckInputStream.hpp"

namespace checks
{
	CheckInputStream::CheckInputStream(const std::unique_ptr<wrappers::InputVoice>& input) :
		input(input)
	{

	}

	bool CheckInputStream::check() const
	{
		return static_cast<bool>(input);
	}

	std::string_view CheckInputStream::getErrorMessage() const
	{
		constexpr std::string_view errorMessage = "Input not initialized";

		return errorMessage;
	}

	Check::AvailableChecks CheckInputStream::getCheck() const
	{
		return AvailableChecks::inputStream;
	}
}
