#pragma once

#include "Command.hpp"

#include "Wrappers/OutputVoice.hpp"

namespace commands
{
	class OverrideOutputDeviceId : public Command
	{
	private:
		std::unique_ptr<wrappers::OutputVoice>& output;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		OverrideOutputDeviceId(std::unique_ptr<wrappers::OutputVoice>& output, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~OverrideOutputDeviceId() = default;
	};
}
