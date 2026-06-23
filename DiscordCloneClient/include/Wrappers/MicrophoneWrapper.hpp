#pragma once

#include "SocketWrapper.hpp"

namespace wrappers
{
	class MicrophoneWrapper
	{
	private:
		MicrophoneObject implementation;

	public:
		MicrophoneWrapper(SocketWrapper& socket);

		void overrideDeviceId(uint32_t id);

		void muteOrUnmute();

		bool isStreamRunning() const;

		void setVolume(double volume);

		double getVolume() const;

		~MicrophoneWrapper();
	};
}
