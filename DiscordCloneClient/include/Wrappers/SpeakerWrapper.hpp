#pragma once

#include "SocketWrapper.hpp"

namespace wrappers
{
	class SpeakerWrapper
	{
	private:
		SpeakerObject implementation;

	public:
		SpeakerWrapper(SocketWrapper& socket);

		void overrideDeviceId(uint32_t id);

		void setVolume(double volume);

		double getVolume() const;

		~SpeakerWrapper();
	};
}
