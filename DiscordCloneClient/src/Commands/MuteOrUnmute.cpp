#include "Commands/MuteOrUnmute.hpp"

constexpr std::string_view commandName = "mute_or_unmute";

namespace commands
{
	bool MuteOrUnmute::run(std::istream& stream)
	{
		input->muteOrUnmute();

		return true;
	}

	uint32_t MuteOrUnmute::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	MuteOrUnmute::MuteOrUnmute(const std::unique_ptr<wrappers::InputVoice>& input, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		input(input)
	{

	}
}
