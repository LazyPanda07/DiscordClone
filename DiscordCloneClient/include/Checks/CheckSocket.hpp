#pragma once

#include "Check.hpp"

#include <memory>

#include "Wrappers/SocketWrapper.hpp"

namespace checks
{
	class CheckSocket : public Check
	{
	private:
		const std::unique_ptr<wrappers::SocketWrapper>& socket;

	public:
		CheckSocket(const std::unique_ptr<wrappers::SocketWrapper>& socket);

		bool check() const override;

		std::string_view getErrorMessage() const override;

		AvailableChecks getCheck() const override;

		~CheckSocket() = default;
	};
}
