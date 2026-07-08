#pragma once

#include "Command.hpp"

#include "Wrappers/SpeakerWrapper.hpp"

namespace commands
{
	class FixSpeakerDelay : public Command
	{
	private:
		const std::unique_ptr<wrappers::SpeakerWrapper>& speaker;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		FixSpeakerDelay(const std::unique_ptr<wrappers::SpeakerWrapper>& speaker, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~FixSpeakerDelay() = default;
	};
}
