#include "Commands/SetMicrophoneVolume.hpp"

constexpr std::string_view commandName = "set_microphone_volume";

namespace commands
{
	bool SetMicrophoneVolume::run(std::istream& stream)
	{
		double volume;

		stream >> volume;

		microphone->setVolume(volume);

		settings.modifySettings
		(
			[volume](client::Settings& self)
			{
				self.inputVolume = volume;
			}
		);

		return true;
	}

	uint32_t SetMicrophoneVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	SetMicrophoneVolume::SetMicrophoneVolume(std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		microphone(microphone),
		settings(settings)
	{

	}

	std::string_view SetMicrophoneVolume::getHelpText() const
	{
		constexpr std::string_view helpText = "<volume>";

		return helpText;
	}
}
