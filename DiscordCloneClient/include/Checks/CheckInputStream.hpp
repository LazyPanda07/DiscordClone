#pragma once

#include "Check.hpp"

#include <memory>

#include "Wrappers/InputVoice.hpp"

namespace checks
{
	class CheckInputStream : public Check
	{
	private:
		const std::unique_ptr<wrappers::InputVoice>& input;

	public:
		CheckInputStream(const std::unique_ptr<wrappers::InputVoice>& input);

		bool check() const override;

		std::string_view getErrorMessage() const override;

		AvailableChecks getCheck() const override;

		~CheckInputStream() = default;
	};
}
