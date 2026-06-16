#include "Commands/GetInputVolume.hpp"

#include <iostream>

constexpr std::string_view commandName = "get_input_volume";

namespace commands
{
	bool GetInputVolume::run(std::istream& stream)
	{
		std::cout << input->getVolume() << std::endl;

		return true;
	}

	uint32_t GetInputVolume::getChecks() const
	{
		return checks::Check::AvailableChecks::inputStream;
	}

	GetInputVolume::GetInputVolume(const std::unique_ptr<wrappers::InputVoice>& input, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks),
		input(input)
	{

	}
}
