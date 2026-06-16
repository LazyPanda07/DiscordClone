#pragma once

#include "Command.hpp"

namespace commands
{
	class Exit : public Command
	{
	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		Exit(const std::vector<std::unique_ptr<checks::Check>>& checks);

		~Exit() = default;
	};
}
