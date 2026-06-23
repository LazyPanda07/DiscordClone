#include "Commands/GetMicrophoneVolume.hpp"

#include <iostream>

constexpr std::string_view commandName = "get_microphone_volume";

namespace commands
{
	bool GetMicrophoneVolume::run(std::istream& stream)
	{
		std::cout << microphone->getVolume() << std::endl;

		return true;
	}

	uint32_t GetMicrophoneVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	GetMicrophoneVolume::GetMicrophoneVolume(const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		microphone(microphone)
	{

	}
}
