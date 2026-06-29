#pragma once

#include <array>

#include <UDPSocket.hpp>

namespace constants
{
	inline constexpr std::array<float, web::UDPSocket::voicePacketSize / sizeof(float)> silence{};
}
