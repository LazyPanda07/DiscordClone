#include <iostream>
#include <thread>
#include <chrono>

#include <import.hpp>
#include <ConsoleArgumentParser.h>

int main(int argc, char** argv) try
{
	framework::utility::initializeWebFramework();

	utility::parsers::ConsoleArgumentParser argumentsParser(argc, argv);

	std::string ip = argumentsParser.get<std::string>("ip", "127.0.0.1");
	uint16_t port = argumentsParser.get<uint16_t>("port", 8080);

	framework::utility::Config config("config.json");

	config.overrideConfiguration("port", port);
	config.overrideConfiguration("ip", ip);

	framework::WebFramework server(config);

	server.start(true, [&config]() { std::cout << std::format("Server is running: on {}:{}", config.get<std::string>("ip"), config.get<int32_t>("port")) << std::endl; });

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

	return 1;
}
