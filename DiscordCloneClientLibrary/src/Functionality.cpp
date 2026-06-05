#include "Functionality.hpp"

namespace functionality
{
	void muteOrUnmute(voice::InputVoice& input)
	{
		if (input.isStreamRunning())
		{
			input.stopStream();
		}
		else
		{
			input.startStream();
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
		constexpr std::string_view version = "0.4.0";

		return version.data();
	}
}
