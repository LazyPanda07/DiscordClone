#include "Commands/GetOutputVolume.hpp"

#include <iostream>

constexpr std::string_view commandName = "get_output_volume";

namespace commands
{
	bool GetOutputVolume::run(std::istream& stream)
	{
		std::cout << output->getVolume() << std::endl;

		return true;
	}

	uint32_t GetOutputVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::outputStream;
	}

	GetOutputVolume::GetOutputVolume(const std::unique_ptr<wrappers::OutputVoice>& output, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		output(output)
	{

	}
}
