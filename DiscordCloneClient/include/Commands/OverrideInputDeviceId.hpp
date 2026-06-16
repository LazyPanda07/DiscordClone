#pragma once

#include "Command.hpp"

#include "Wrappers/InputVoice.hpp"

namespace commands
{
	class OverrideInputDeviceId : public Command
	{
	private:
		std::unique_ptr<wrappers::InputVoice>& input;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		OverrideInputDeviceId(std::unique_ptr<wrappers::InputVoice>& input, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~OverrideInputDeviceId() = default;
	};
}
