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
}
