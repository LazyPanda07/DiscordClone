#include <iostream>

#include <import.hpp>
#include <ConsoleArgumentParser.h>

int main(int argc, char** argv) try
{
	framework::utility::initializeWebFramework();

	utility::parsers::ConsoleArgumentParser argumentsParser(argc, argv);

	framework::utility::Config config("config.json");

	std::string ip = argumentsParser.get<std::string>("ip");
	uint16_t port = argumentsParser.get<uint16_t>("port");

	if (ip.size())
	{
		config.overrideConfiguration("ip", ip);
	}

	if (port != 0)
	{
		config.overrideConfiguration("port", port);
	}

	framework::WebFramework server(config);

	server.start(true, [&config]() { std::cout << std::format("Server is running: on {}:{}", config.get<std::string>("ip"), config.get<int32_t>("port")) << std::endl; });

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

	return 1;
}
