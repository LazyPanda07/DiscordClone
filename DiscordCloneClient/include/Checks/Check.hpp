#pragma once

#include <string_view>
#include <cstdint>

namespace checks
{
	class Check
	{
	public:
		enum AvailableChecks : uint32_t
		{
			inputStream = 1,
			outputStream = inputStream << 1,
			socketDatagram = outputStream << 1,
			socketStream = socketDatagram << 1
		};

	public:
		Check() = default;

		virtual bool check() const = 0;

		virtual std::string_view getErrorMessage() const = 0;

		virtual AvailableChecks getCheck() const = 0;

		virtual ~Check() = default;
	};
}
