#pragma once

#include "Check.hpp"

#include <memory>

#include "Wrappers/SpeakerWrapper.hpp"

namespace checks
{
	class CheckSpeaker : public Check
	{
	private:
		const std::unique_ptr<wrappers::SpeakerWrapper>& speaker;

	public:
		CheckSpeaker(const std::unique_ptr<wrappers::SpeakerWrapper>& speaker);

		bool check() const override;

		std::string_view getErrorMessage() const override;

		AvailableChecks getCheck() const override;

		~CheckSpeaker() = default;
	};
}
