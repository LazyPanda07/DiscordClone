#pragma once

#include "Command.hpp"

#include "Wrappers/SocketWrapper.hpp"

namespace commands
{
	class Echo : public Command
	{
	private:
		const std::unique_ptr<wrappers::SocketWrapper>& socket;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		Echo(const std::unique_ptr<wrappers::SocketWrapper>& socket, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~Echo() = default;
	};
}
