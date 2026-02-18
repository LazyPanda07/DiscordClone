#pragma once

#include "InputVoice.hpp"
#include "OutputVoice.hpp"

namespace functionality
{
	void muteOrUnmute(voice::InputVoice& input);

	std::vector<RtAudio::DeviceInfo> getAudioDevices();

	std::string_view getDiscordCloneVersion();
}
