#pragma once

#include "Command.hpp"

#include "Wrappers/InputVoice.hpp"

namespace commands
{
	class MuteOrUnmute : public Command
	{
	private:
		const std::unique_ptr<wrappers::InputVoice>& input;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		MuteOrUnmute(const std::unique_ptr<wrappers::InputVoice>& input, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~MuteOrUnmute() = default;
	};
}
