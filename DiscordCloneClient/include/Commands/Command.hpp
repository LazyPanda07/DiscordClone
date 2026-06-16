#pragma once

#include <string>
#include <vector>
#include <memory>
#include <istream>

#include "Checks/Check.hpp"

namespace commands
{
	class Command
	{
	public:
		const std::string command;

	private:
		const std::vector<std::unique_ptr<checks::Check>>& checks;

	protected:
		virtual bool run(std::istream& stream) = 0;

		virtual uint32_t getChecks() const = 0;

	public:
		Command(std::string_view command, const std::vector<std::unique_ptr<checks::Check>>& checks);

		bool conditionalRun(std::istream& stream);

		virtual std::string_view getHelpText() const;

		virtual ~Command() = default;
	};
}
