#include <iostream>
#include <sstream>
#include <vector>

#include <UDPSocket.hpp>

int main(int argc, char** argv) try
{
	std::string command;
	std::unique_ptr<web::UDPSocket> socket;
	std::vector<std::pair<std::string, std::string>> availableCommands =
	{
		{ "send", "<message>" },
		{ "connect", "<ip:port>" },
		{ "help", "" },
		{ "exit", "" }
	};

	while (true)
	{
		std::cout << "Enter command: ";

		std::getline(std::cin, command);

		if (!command.find("send"))
		{
			std::istringstream is(command);
			std::string message;

			is >> command;

			std::getline(is, message);

			message.erase(message.begin());

			if (socket->sendData(message) == SOCKET_ERROR)
			{
				std::cout << WSAGetLastError() << std::endl;
			}
		}
		else if (!command.find("connect"))
		{
			std::istringstream is(command);
			std::string ip;
			std::string port;

			is >> command;

			std::getline(is, ip, ':');

			ip.erase(ip.begin());

			is >> port;

			socket = std::make_unique<web::UDPSocket>(ip.data(), static_cast<uint16_t>(std::stoi(port)));

			std::cout << ip << ':' << port << std::endl;
		}
		else if (!command.find("help"))
		{
			for (const auto& [command, message] : availableCommands)
			{
				std::cout << command << ": " << message << std::endl;
			}
		}
		else if (!command.find("exit"))
		{
			return 0;
		}
	}

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

	return 1;
}
