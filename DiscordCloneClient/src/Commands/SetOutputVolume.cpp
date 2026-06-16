#include "Commands/SetOutputVolume.hpp"

constexpr std::string_view commandName = "set_output_volume";

namespace commands
{
	bool SetOutputVolume::run(std::istream& stream)
	{
		double volume;

		stream >> volume;

		output->setVolume(volume);

		settings.modifySettings
		(
			[volume](client::Settings& self)
			{
				self.outputVolume = volume;
			}
		);

		return true;
	}

	uint32_t SetOutputVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::outputStream;
	}

	SetOutputVolume::SetOutputVolume(std::unique_ptr<wrappers::OutputVoice>& output, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		output(output),
		settings(settings)
	{

	}

	std::string_view SetOutputVolume::getHelpText() const
	{
		constexpr std::string_view helpText = "<volume>";

		return helpText;
	}
}
