#include <iostream>
#include <thread>
#include <chrono>

#include <UDPServerSocket.hpp>
#include <import.hpp>
#include <ConsoleArgumentParser.h>

#include "VoiceServer.hpp"

int main(int argc, char** argv) try
{
	framework::utility::initializeWebFramework();

	utility::parsers::ConsoleArgumentParser argumentsParser(argc, argv);

	std::unique_ptr<web::UDPSocket> socket;
	voice::VoiceServer voiceServer;
	web::UDPSocket::ReceiveCallback callback(voiceServer);
	std::string ip = argumentsParser.get<std::string>("ip", "127.0.0.1");
	uint16_t port = argumentsParser.get<uint16_t>("port", 8080);

	framework::utility::Config config("config.json");

	config.overrideConfiguration("port", port);
	config.overrideConfiguration("ip", ip);

	framework::WebFramework server(config);

	server.start(false, [&config]() { std::cout << std::format("Server is running: on {}:{}", config.get<std::string>("ip"), config.get<int32_t>("port")) << std::endl; });

	port++;

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
