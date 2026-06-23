#include "Checks/CheckMicrophone.hpp"

namespace checks
{
	CheckMicrophone::CheckMicrophone(const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone) :
		microphone(microphone)
	{

	}

	bool CheckMicrophone::check() const
	{
		return static_cast<bool>(microphone);
	}

	std::string_view CheckMicrophone::getErrorMessage() const
	{
		constexpr std::string_view errorMessage = "Input not initialized";

		return errorMessage;
	}

	Check::AvailableChecks CheckMicrophone::getCheck() const
	{
		return AvailableChecks::inputStream;
	}
}
