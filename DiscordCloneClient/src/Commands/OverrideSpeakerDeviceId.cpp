#include "Commands/OverrideSpeakerDeviceId.hpp"

constexpr std::string_view commandName = "override_speaker_device_id";

namespace commands
{
	bool OverrideSpeakerDeviceId::run(std::istream& stream)
	{
		uint32_t id;

		stream >> id;

		speaker->overrideDeviceId(id);

		return true;
	}

	uint32_t OverrideSpeakerDeviceId::getChecks() const
	{
		return checks::Check::AvailableChecks::outputStream;
	}

	OverrideSpeakerDeviceId::OverrideSpeakerDeviceId(std::unique_ptr<wrappers::SpeakerWrapper>& speaker, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		speaker(speaker)
	{

	}

	std::string_view OverrideSpeakerDeviceId::getHelpText() const
	{
		constexpr std::string_view helpText = "<id>";

		return helpText;
	}
}
