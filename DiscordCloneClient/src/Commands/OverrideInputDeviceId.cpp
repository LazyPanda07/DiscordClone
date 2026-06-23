#include "Commands/OverrideInputDeviceId.hpp"

constexpr std::string_view commandName = "override_input_device_id";

namespace commands
{
	bool OverrideInputDeviceId::run(std::istream& stream)
	{
		uint32_t id;

		stream >> id;

		microphone->overrideDeviceId(id);

		return true;
	}

	uint32_t OverrideInputDeviceId::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	OverrideInputDeviceId::OverrideInputDeviceId(std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		microphone(microphone)
	{

	}

	std::string_view OverrideInputDeviceId::getHelpText() const
	{
		constexpr std::string_view helpText = "<id>";

		return helpText;
	}
}
