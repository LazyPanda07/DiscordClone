#pragma once

#include <string>

#include <c_api.h>

namespace wrappers
{
	class SocketWrapper
	{
	private:
		UdpSocketObject implementation;

	public:
		SocketWrapper(std::string_view ip, uint16_t port);

		void sendData(std::string_view data);

		std::string receiveData(int32_t flags = 0);

		~SocketWrapper();

		friend class MicrophoneWrapper;
		friend class SpeakerWrapper;
	};
}
