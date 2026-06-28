#include "Functionality.hpp"

namespace functionality
{
	void muteOrUnmute(voice::Microphone& microphone)
	{
		if (microphone.isStreamRunning())
		{
			microphone.stopStream();
		}
		else
		{
			microphone.startStream();
		}
	}

	std::vector<RtAudio::DeviceInfo> getAudioDevices()
	{
		RtAudio audio;
		std::vector<RtAudio::DeviceInfo> result;

		for (uint32_t device : audio.getDeviceIds())
		{
			result.emplace_back(audio.getDeviceInfo(device));
		}

		return result;
	}

	std::string_view getDiscordCloneClientLibraryVersion()
	{
		constexpr std::string_view version = "0.6.2";

		return version.data();
	}
}
