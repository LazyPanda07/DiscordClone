#include <iostream>
#include <thread>
#include <chrono>

#include <UDPServerSocket.hpp>

#include "VoiceServer.hpp"

int main(int argc, char** argv) try
{
	std::unique_ptr<web::UDPSocket> socket;
	voice::VoiceServer server;
	web::UDPSocket::ReceiveCallback callback(server);
	int16_t port;

	if (argc == 1)
	{
		port = 8080;
	}
	else if (argc == 2)
	{
		port = static_cast<uint16_t>(std::stoi(argv[1]));
	}
	else
	{
		throw std::runtime_error(std::format("Only supports: {0} or {0} <port>", argv[0]));
	}

	socket = std::make_unique<web::UDPServerSocket>(port);

	std::cout << std::format("Receive datagrams on {} port", port) << std::endl;

	while (true)
	{
		socket->receiveData(callback);
	}

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

	return 1;
}
