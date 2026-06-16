#pragma once

#include "Check.hpp"

#include <memory>

#include "Wrappers/OutputVoice.hpp"

namespace checks
{
	class CheckOutputStream : public Check
	{
	private:
		const std::unique_ptr<wrappers::OutputVoice>& input;

	public:
		CheckOutputStream(const std::unique_ptr<wrappers::OutputVoice>& input);

		bool check() const override;

		std::string_view getErrorMessage() const override;

		AvailableChecks getCheck() const override;

		~CheckOutputStream() = default;
	};
}
