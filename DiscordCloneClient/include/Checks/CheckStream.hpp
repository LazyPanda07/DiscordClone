#pragma once

#include "Check.hpp"

#include <memory>

#include <IOSocketStream.h>

namespace checks
{
	class CheckStream : public Check
	{
	private:
		const std::unique_ptr<streams::IOSocketStream>& stream;

	public:
		CheckStream(const std::unique_ptr<streams::IOSocketStream>& stream);

		bool check() const override;

		std::string_view getErrorMessage() const override;

		AvailableChecks getCheck() const override;

		~CheckStream() = default;
	};
}
