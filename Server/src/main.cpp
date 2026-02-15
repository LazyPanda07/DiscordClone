#include <iostream>
#include <thread>
#include <chrono>

#include <UDPServerSocket.hpp>

#include "VoiceServer.hpp"

int main(int argc, char** argv)
{
	std::unique_ptr<web::UDPSocket> socket = std::make_unique<web::UDPServerSocket>(8080);
	voice::VoiceServer server;

	while (true)
	{
		socket->receiveData(server);
	}

	return 0;
}
