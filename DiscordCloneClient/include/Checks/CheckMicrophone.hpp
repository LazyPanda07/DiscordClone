#pragma once

#include "Check.hpp"

#include <memory>

#include "Wrappers/MicrophoneWrapper.hpp"

namespace checks
{
	class CheckMicrophone : public Check
	{
	private:
		const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone;

	public:
		CheckMicrophone(const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone);

		bool check() const override;

		std::string_view getErrorMessage() const override;

		AvailableChecks getCheck() const override;

		~CheckMicrophone() = default;
	};
}
