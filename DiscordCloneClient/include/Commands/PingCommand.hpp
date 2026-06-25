#pragma once

#include "Command.hpp"

#include <IOSocketStream.h>

namespace commands
{
	class PingCommand : public Command
	{
	private:
		const std::unique_ptr<streams::IOSocketStream>& controlStream;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		PingCommand(const std::unique_ptr<streams::IOSocketStream>& controlStream, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~PingCommand() = default;
	};
}
