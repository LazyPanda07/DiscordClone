#pragma once

#include "SocketWrapper.hpp"

namespace wrappers
{
	class OutputVoice
	{
	private:
		OutputVoiceStreamObject implementation;

	public:
		OutputVoice(SocketWrapper& socket);

		void overrideDeviceId(uint32_t id);

		void setVolume(double volume);

		double getVolume() const;

		~OutputVoice();
	};
}
