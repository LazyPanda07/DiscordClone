#include "Commands/OverrideOutputDeviceId.hpp"

constexpr std::string_view commandName = "override_output_device_id";

namespace commands
{
	bool OverrideOutputDeviceId::run(std::istream& stream)
	{
		uint32_t id;

		stream >> id;

		speaker->overrideDeviceId(id);

		return true;
	}

	uint32_t OverrideOutputDeviceId::getChecks() const
	{
		return checks::Check::AvailableChecks::outputStream;
	}

	OverrideOutputDeviceId::OverrideOutputDeviceId(std::unique_ptr<wrappers::SpeakerWrapper>& speaker, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		speaker(speaker)
	{

	}

	std::string_view OverrideOutputDeviceId::getHelpText() const
	{
		constexpr std::string_view helpText = "<id>";

		return helpText;
	}
}
