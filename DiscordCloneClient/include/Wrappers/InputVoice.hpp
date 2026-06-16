#pragma once

#include "SocketWrapper.hpp"

namespace wrappers
{
	class InputVoice
	{
	private:
		InputVoiceStreamObject implementation;

	public:
		InputVoice(SocketWrapper& socket);

		void overrideDeviceId(uint32_t id);

		void muteOrUnmute();

		bool isStreamRunning() const;

		void setVolume(double volume);

		double getVolume() const;

		~InputVoice();
	};
}
