#pragma once

#include "Command.hpp"

#include "Wrappers/MicrophoneWrapper.hpp"

namespace commands
{
	class GetMicrophoneVolume : public Command
	{
	private:
		const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		GetMicrophoneVolume(const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~GetMicrophoneVolume() = default;
	};
}
