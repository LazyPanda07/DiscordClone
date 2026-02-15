#include <iostream>
#include <thread>
#include <chrono>

#include <UDPServerSocket.hpp>

#include "VoiceServer.hpp"

int main(int argc, char** argv)
{
	std::unique_ptr<web::UDPSocket> socket;
	voice::VoiceServer server;
	web::UDPSocket::ReceiveCallback callback(server);

	if (argc == 1)
	{
		socket = std::make_unique<web::UDPServerSocket>(8080);
	}
	else
	{
		socket = std::make_unique<web::UDPServerSocket>(static_cast<uint16_t>(std::stoi(argv[1])));
	}

	while (true)
	{
		socket->receiveData(callback);
	}

	return 0;
}
