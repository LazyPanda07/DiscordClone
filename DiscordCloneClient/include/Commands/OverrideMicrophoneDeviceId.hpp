#pragma once

#include "Command.hpp"

#include "Wrappers/MicrophoneWrapper.hpp"

namespace commands
{
	class OverrideMicrophoneDeviceId : public Command
	{
	private:
		std::unique_ptr<wrappers::MicrophoneWrapper>& microphone;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		OverrideMicrophoneDeviceId(std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~OverrideMicrophoneDeviceId() = default;
	};
}
