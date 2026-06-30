#pragma once

#include "Command.hpp"

namespace commands
{
	class GetVersion : public Command
	{
	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		GetVersion(const std::vector<std::unique_ptr<checks::Check>>& checks);

		~GetVersion() = default;
	};
}
