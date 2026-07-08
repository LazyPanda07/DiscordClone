#include "Commands/FixSpeakerDelay.hpp"

constexpr std::string_view commandName = "fix_speaker_delay";

namespace commands
{
	bool FixSpeakerDelay::run(std::istream& stream)
	{
		speaker->fixDelay();

		return true;
	}

	uint32_t FixSpeakerDelay::getChecks() const
	{
		return checks::Check::AvailableChecks::speaker;
	}

	FixSpeakerDelay::FixSpeakerDelay(const std::unique_ptr<wrappers::SpeakerWrapper>& speaker, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		speaker(speaker)
	{

	}
}
