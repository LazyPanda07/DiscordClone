#pragma once

#include "Command.hpp"

#include "Wrappers/MicrophoneWrapper.hpp"
#include "Settings.hpp"

namespace commands
{
	class SetMicrophoneVolume : public Command
	{
	private:
		std::unique_ptr<wrappers::MicrophoneWrapper>& microphone;
		client::Settings& settings;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		SetMicrophoneVolume(std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~SetMicrophoneVolume() = default;
	};
}
