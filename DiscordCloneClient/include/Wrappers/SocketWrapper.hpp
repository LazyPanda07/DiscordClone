#pragma once

#include <string>

#include <c_api.h>

namespace wrapper
{
	class SocketWrapper
	{
	private:
		UdpSocketObject implementation;

	public:
		SocketWrapper(std::string_view ip, uint16_t port);

		void sendData(std::string_view data);

		std::string receiveData();

		~SocketWrapper();

		friend class InputVoice;
		friend class OutputVoice;
	};
}
