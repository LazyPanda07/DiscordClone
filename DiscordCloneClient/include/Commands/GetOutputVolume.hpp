#pragma once

#include "Command.hpp"

#include "Wrappers/OutputVoice.hpp"

namespace commands
{
	class GetOutputVolume : public Command
	{
	private:
		const std::unique_ptr<wrappers::OutputVoice>& output;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		GetOutputVolume(const std::unique_ptr<wrappers::OutputVoice>& output, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~GetOutputVolume() = default;
	};
}
