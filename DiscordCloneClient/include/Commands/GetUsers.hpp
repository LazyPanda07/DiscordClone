#pragma once

#include "Command.hpp"

#include <IOSocketStream.h>

#include "Settings.hpp"

namespace commands
{
	class GetUsers : public Command
	{
	private:
		const std::unique_ptr<streams::IOSocketStream>& controlStream;
		const client::Settings& settings;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		GetUsers(const std::unique_ptr<streams::IOSocketStream>& controlStream, const client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~GetUsers() = default;
	};
}
