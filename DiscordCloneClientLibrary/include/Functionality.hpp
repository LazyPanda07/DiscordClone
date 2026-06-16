#pragma once

#include <algorithm>

#include "InputVoice.hpp"
#include "OutputVoice.hpp"

namespace functionality
{
	void muteOrUnmute(voice::InputVoice& input);

	std::vector<RtAudio::DeviceInfo> getAudioDevices();

	std::string_view getDiscordCloneClientLibraryVersion();

	template<size_t SizeT>
	void toInt(const std::span<float>& in, std::array<int16_t, SizeT>& out);

	template<size_t SizeT>
	void toFloat(const std::array<int16_t, SizeT>& in, std::array<float, SizeT>& out);
}

namespace functionality
{
	template<size_t SizeT>
	void toInt(const std::span<float>& in, std::array<int16_t, SizeT>& out)
	{
		for (size_t i = 0; i < SizeT; i++)
		{
			out[i] = static_cast<int16_t>(std::clamp(in[i], -1.0f, 1.0f) * static_cast<float>(std::numeric_limits<int16_t>::max()));
		}
	}

	template<size_t SizeT>
	void toFloat(const std::array<int16_t, SizeT>& in, std::array<float, SizeT>& out)
	{
		constexpr float normalizeConstant = 1.0f / static_cast<float>(std::numeric_limits<int16_t>::max() + 1);

		for (size_t i = 0; i < SizeT; i++)
		{
			out[i] = in[i] * normalizeConstant;
		}
	}
}
