#pragma once

#include "Command.hpp"

#include "Wrappers/SpeakerWrapper.hpp"

namespace commands
{
	class OverrideSpeakerDeviceId : public Command
	{
	private:
		std::unique_ptr<wrappers::SpeakerWrapper>& speaker;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		OverrideSpeakerDeviceId(std::unique_ptr<wrappers::SpeakerWrapper>& speaker, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~OverrideSpeakerDeviceId() = default;
	};
}
