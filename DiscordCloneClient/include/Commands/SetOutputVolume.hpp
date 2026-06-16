#pragma once

#include "Command.hpp"

#include "Wrappers/OutputVoice.hpp"
#include "Settings.hpp"

namespace commands
{
	class SetOutputVolume : public Command
	{
	private:
		std::unique_ptr<wrappers::OutputVoice>& output;
		client::Settings& settings;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		SetOutputVolume(std::unique_ptr<wrappers::OutputVoice>& output, client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~SetOutputVolume() = default;
	};
}
