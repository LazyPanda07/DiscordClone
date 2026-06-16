#include "Commands/SetInputVolume.hpp"

constexpr std::string_view commandName = "set_input_volume";

namespace commands
{
	bool SetInputVolume::run(std::istream& stream)
	{
		double volume;

		stream >> volume;

		input->setVolume(volume);

		settings.modifySettings
		(
			[volume](client::Settings& self)
			{
				self.inputVolume = volume;
			}
		);

		return true;
	}

	uint32_t SetInputVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	SetInputVolume::SetInputVolume(std::unique_ptr<wrappers::InputVoice>& input, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		input(input),
		settings(settings)
	{

	}

	std::string_view SetInputVolume::getHelpText() const
	{
		constexpr std::string_view helpText = "<volume>";

		return helpText;
	}
}
