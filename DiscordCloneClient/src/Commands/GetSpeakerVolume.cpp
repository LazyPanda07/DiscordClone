#include "Commands/GetSpeakerVolume.hpp"

#include <iostream>

constexpr std::string_view commandName = "get_speaker_volume";

namespace commands
{
	bool GetSpeakerVolume::run(std::istream& stream)
	{
		std::cout << speaker->getVolume() << std::endl;

		return true;
	}

	uint32_t GetSpeakerVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::outputStream;
	}

	GetSpeakerVolume::GetSpeakerVolume(const std::unique_ptr<wrappers::SpeakerWrapper>& speaker, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		speaker(speaker)
	{

	}
}
