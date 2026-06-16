#pragma once

#include "Command.hpp"

#include "Wrappers/InputVoice.hpp"
#include "Settings.hpp"

namespace commands
{
	class SetInputVolume : public Command
	{
	private:
		std::unique_ptr<wrappers::InputVoice>& input;
		client::Settings& settings;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		SetInputVolume(std::unique_ptr<wrappers::InputVoice>& input, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~SetInputVolume() = default;
	};
}
