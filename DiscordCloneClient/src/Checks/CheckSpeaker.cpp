#include "Checks/CheckSpeaker.hpp"

namespace checks
{
	CheckSpeaker::CheckSpeaker(const std::unique_ptr<wrappers::SpeakerWrapper>& speaker) :
		speaker(speaker)
	{

	}

	bool CheckSpeaker::check() const
	{
		return static_cast<bool>(speaker);
	}

	std::string_view CheckSpeaker::getErrorMessage() const
	{
		constexpr std::string_view errorMessage = "Output not initialized";

		return errorMessage;
	}

	Check::AvailableChecks CheckSpeaker::getCheck() const
	{
		return AvailableChecks::outputStream;
	}
}
