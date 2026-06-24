#include "Commands/OverrideMicrophoneDeviceId.hpp"

constexpr std::string_view commandName = "override_microphone_device_id";

namespace commands
{
	bool OverrideMicrophoneDeviceId::run(std::istream& stream)
	{
		uint32_t id;

		stream >> id;

		microphone->overrideDeviceId(id);

		return true;
	}

	uint32_t OverrideMicrophoneDeviceId::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	OverrideMicrophoneDeviceId::OverrideMicrophoneDeviceId(std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		microphone(microphone)
	{

	}

	std::string_view OverrideMicrophoneDeviceId::getHelpText() const
	{
		constexpr std::string_view helpText = "<id>";

		return helpText;
	}
}
