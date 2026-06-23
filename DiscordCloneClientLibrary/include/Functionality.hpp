#pragma once

#include <algorithm>

#include "Microphone.hpp"
#include "Speaker.hpp"

namespace functionality
{
	void muteOrUnmute(voice::Microphone& microphone);

	std::vector<RtAudio::DeviceInfo> getAudioDevices();

	std::string_view getDiscordCloneClientLibraryVersion();
}
