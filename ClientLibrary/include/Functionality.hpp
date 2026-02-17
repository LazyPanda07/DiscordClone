#pragma once

#include <vector>

#include "InputVoice.hpp"
#include "OutputVoice.hpp"

namespace functionality
{
	void muteOrUnmute(voice::InputVoice& input);

	void overrideInputDeviceId(voice::InputVoice& input, uint32_t id);

	void overrideOutputDevice(voice::OutputVoice& output, uint32_t id);

	std::vector<RtAudio::DeviceInfo> getAudioDevices();
}
