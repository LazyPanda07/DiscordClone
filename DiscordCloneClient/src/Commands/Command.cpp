#include "Commands/Command.hpp"

#include <iostream>

namespace commands
{
	Command::Command(std::string_view command, const std::vector<std::unique_ptr<checks::Check>>& checks) :
		command(command),
		checks(checks)
	{

	}

	bool Command::conditionalRun(std::istream& stream)
	{
		uint32_t runChecks = this->getChecks();
		bool passChecks = true;

		for (const std::unique_ptr<checks::Check>& check : checks)
		{
			if (runChecks & check->getCheck())
			{
				bool temp = check->check();

				passChecks &= temp;

				if (!temp)
				{
					std::cerr << check->getErrorMessage() << std::endl;
				}
			}
		}

		if (passChecks)
		{
			return this->run(stream);
		}

		return false;
	}

	std::string_view Command::getHelpText() const
	{
		constexpr std::string_view empty;

		return empty;
	}
}
