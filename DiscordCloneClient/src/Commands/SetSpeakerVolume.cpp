#include "Commands/SetSpeakerVolume.hpp"

constexpr std::string_view commandName = "set_speaker_volume";

namespace commands
{
	bool SetSpeakerVolume::run(std::istream& stream)
	{
		double volume;

		stream >> volume;

		speaker->setVolume(volume);

		settings.modifySettings
		(
			[volume](client::Settings& self)
			{
				self.speakerVolume = volume;
			}
		);

		return true;
	}

	uint32_t SetSpeakerVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::outputStream;
	}

	SetSpeakerVolume::SetSpeakerVolume(std::unique_ptr<wrappers::SpeakerWrapper>& speaker, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		speaker(speaker),
		settings(settings)
	{

	}

	std::string_view SetSpeakerVolume::getHelpText() const
	{
		constexpr std::string_view helpText = "<volume>";

		return helpText;
	}
}
