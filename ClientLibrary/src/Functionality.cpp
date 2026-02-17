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

	void muteOrUnmuteoverrideInputDeviceId(voice::InputVoice& input, uint32_t id)
	{

	}

	void overrideOutputDevice(voice::OutputVoice& output, uint32_t id)
	{

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
}
