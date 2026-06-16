#include "Commands/Exit.hpp"

constexpr std::string_view commandName = "exit";

namespace commands
{
	bool Exit::run(std::istream& stream)
	{
		exit(0);

		return true;
	}

	uint32_t Exit::getChecks() const
	{
		return NULL;
	}

	Exit::Exit(const std::vector<std::unique_ptr<checks::Check>>& checks) :
		Command(commandName, checks)
	{

	}
}
