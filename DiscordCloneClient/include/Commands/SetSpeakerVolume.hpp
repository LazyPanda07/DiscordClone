#pragma once

#include "Command.hpp"

#include "Wrappers/SpeakerWrapper.hpp"
#include "Settings.hpp"

namespace commands
{
	class SetSpeakerVolume : public Command
	{
	private:
		std::unique_ptr<wrappers::SpeakerWrapper>& speaker;
		client::Settings& settings;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		SetSpeakerVolume(std::unique_ptr<wrappers::SpeakerWrapper>& speaker, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~SetSpeakerVolume() = default;
	};
}
