#pragma once

#include "SocketWrapper.hpp"

namespace wrapper
{
	class InputVoice
	{
	private:
		InputVoiceStreamObject implementation;

	public:
		InputVoice(SocketWrapper& socket);

		void overrideDeviceId(uint32_t id);

		void muteOrUnmute();

		void setVolume(double volume);

		double getVolume() const;

		~InputVoice();
	};
}
