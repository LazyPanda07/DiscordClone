#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

#include <UDPSocket.hpp>
#include <c_api.h>

#include "Hotkeys.hpp"
#include "Settings.hpp"
#include "Wrappers/InputVoice.hpp"
#include "Wrappers/OutputVoice.hpp"
#include "Utils.hpp"

#ifndef __LINUX__
#include <Windows.h>
#endif

std::pair<std::string, uint16_t> parseIpPort(std::string& command);

bool connect(std::string_view ip, uint16_t port, std::unique_ptr<wrapper::SocketWrapper>& resultSocket, std::unique_ptr<wrapper::InputVoice>& input, std::unique_ptr<wrapper::OutputVoice>& output);

void restoreVolume(const client::Settings& settings, const std::unique_ptr<wrapper::InputVoice>& input, const std::unique_ptr<wrapper::OutputVoice>& output);

void printDeviceInfo(const std::unique_ptr<wrapper::InputVoice>& input);

int main(int argc, char** argv) try
{
#ifndef __LINUX__
	SetConsoleOutputCP(CP_UTF8);
#endif

	std::string command;
	std::vector<std::pair<std::string, std::string>> availableCommands =
	{
		{ "connect", "<ip:port>" },
		{ "override_input_device_id", "<id>" },
		{ "override_output_device_id", "<id>" },
		{ "mute_and_unmute", "" },
		{ "set_input_volume", "<volume>" },
		{ "set_output_volume", "<volume>" },
		{ "get_input_volume", "" },
		{ "get_output_volume", "" },
		{ "echo", "" },
		{ "help", "" },
		{ "exit", "" }
	};

	client::Settings settings;
	std::unique_ptr<wrapper::SocketWrapper> socket;
	std::unique_ptr<wrapper::InputVoice> input;
	std::unique_ptr<wrapper::OutputVoice> output;
	functionality::Hotkeys hotkeys;

	if (std::optional<client::Settings> loadedSettings = client::Settings::loadSettings())
	{
		if (connect(loadedSettings->reconnectIp, loadedSettings->reconnectPort, socket, input, output))
		{
			settings = *loadedSettings;
		}
	}

	printDeviceInfo(input);

#ifndef __LINUX__
	hotkeys.registerHotkey
	(
		[&input]()
		{
			input->muteOrUnmute();

			printDeviceInfo(input);
		},
		MOD_CONTROL | MOD_ALT,
		VK_SPACE
	);
#endif

	restoreVolume(settings, input, output);

	while (true)
	{
		std::cout << "Enter command: ";

		std::getline(std::cin, command);

		if (!command.find("connect"))
		{
			using namespace std::chrono_literals;

			const auto& [ip, port] = parseIpPort(command);

			if (connect(ip, port, socket, input, output))
			{
				settings.reconnectIp = ip;
				settings.reconnectPort = port;

				settings.saveSettings();
			}
		}
		else if (!command.find("override_input_device_id"))
		{
			if (!input)
			{
				std::cout << "Input not initialized" << std::endl;

				continue;
			}

			std::istringstream is(command);
			uint32_t id;

			is >> command;
			is >> id;

			input->overrideDeviceId(id);
		}
		else if (!command.find("override_output_device_id"))
		{
			if (!output)
			{
				std::cout << "Output not initialized" << std::endl;

				continue;
			}

			std::istringstream is(command);
			uint32_t id;

			is >> command;
			is >> id;

			output->overrideDeviceId(id);
		}
		else if (!command.find("set_input_volume"))
		{
			if (!input)
			{
				std::cout << "Input not initialized" << std::endl;

				continue;
			}

			std::istringstream is(command);
			double volume;

			is >> command;
			is >> volume;

			input->setVolume(volume);

			settings.inputVolume = volume;

			settings.saveSettings();
		}
		else if (!command.find("set_output_volume"))
		{
			if (!output)
			{
				std::cout << "Output not initialized" << std::endl;

				continue;
			}

			std::istringstream is(command);
			double volume;

			is >> command;
			is >> volume;

			output->setVolume(volume);

			settings.outputVolume = volume;

			settings.saveSettings();
		}
		else if (!command.find("get_input_volume"))
		{
			if (!input)
			{
				std::cout << "Input not initialized" << std::endl;

				continue;
			}

			std::cout << input->getVolume() << std::endl;
		}
		else if (!command.find("get_output_volume"))
		{
			if (!output)
			{
				std::cout << "Output not initialized" << std::endl;

				continue;
			}

			std::cout << output->getVolume() << std::endl;
		}
		else if (!command.find("mute_and_unmute"))
		{
			if (!input)
			{
				std::cout << "Input not initialized" << std::endl;

				continue;
			}

			input->muteOrUnmute();
		}
		else if (!command.find("echo"))
		{
			if (!socket)
			{
				std::cout << "Not connected" << std::endl;

				continue;
			}

			socket->sendData(web::UDPSocket::echo);
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
			exit(0);
		}
	}

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

#ifndef __LINUX__
	std::cin.get();
#endif

	return 1;
}

std::pair<std::string, uint16_t> parseIpPort(std::string& command)
{
	std::istringstream is(command);
	std::string ip;
	std::string port;

	is >> command;

	std::getline(is, ip, ':');

	ip.erase(ip.begin());

	is >> port;

	return std::make_pair(std::move(ip), std::stoi(port));
}

bool connect(std::string_view ip, uint16_t port, std::unique_ptr<wrapper::SocketWrapper>& resultSocket, std::unique_ptr<wrapper::InputVoice>& input, std::unique_ptr<wrapper::OutputVoice>& output)
{
	using namespace std::chrono_literals;

	bool result = false;

	resultSocket = std::make_unique<wrapper::SocketWrapper>(ip, port);

	std::cout << std::format("Connecting to: {}:{}...", ip, port) << std::endl;

	resultSocket->sendData(web::UDPSocket::hello);

	std::this_thread::sleep_for(50ms);

	for (size_t i = 0; i < 5; i++)
	{
		if (resultSocket->receiveData() == web::UDPSocket::hello)
		{
			std::cout << "Connected to: " << ip << ':' << port << std::endl;

			result = true;

			break;
		}

		std::this_thread::sleep_for(1s);

		std::cout << std::format("Connecting to: {}:{}...", ip, port) << std::endl;
	}

	if (result)
	{
		input = std::make_unique<wrapper::InputVoice>(*resultSocket);
		output = std::make_unique<wrapper::OutputVoice>(*resultSocket);
	}
	else
	{
		std::cout << std::format("Failed to connect to: {}:{}", ip, port) << std::endl;
	}

	return result;
}

void restoreVolume(const client::Settings& settings, const std::unique_ptr<wrapper::InputVoice>& input, const std::unique_ptr<wrapper::OutputVoice>& output)
{
	if (!input || !output)
	{
		return;
	}

	input->setVolume(settings.inputVolume);
	output->setVolume(settings.outputVolume);
}

void printDeviceInfo(const std::unique_ptr<wrapper::InputVoice>& input)
{
	DeviceInformationArray devices = utils::callApiFunction(&getDeviceInformation);
	uint64_t size = utils::callApiFunction(&getDeviceInformationSize, devices);

	if (input)
	{
		std::cout << std::endl << std::format("Microphone state: {}", input->isStreamRunning() ? "🟢" : "🔴") << std::endl;
	}

	std::cout << std::format("Found {} audio devices:", size) << std::endl << std::endl;

	for (uint64_t i = 0; i < size; i++)
	{
		uint32_t id = utils::callApiFunction(&getDeviceInformationId, devices, i);
		std::string_view name = utils::callApiFunction(&getDeviceInformationName, devices, i);
		uint32_t inputChannels = utils::callApiFunction(&getDeviceInformationInputChannels, devices, i);
		uint32_t outputChannels = utils::callApiFunction(&getDeviceInformationOutputChannels, devices, i);
		bool isDefaultInput = utils::callApiFunction(&getDeviceInformationDefaultInput, devices, i);
		bool isDefaultOutput = utils::callApiFunction(&getDeviceInformationDefaultOutput, devices, i);

		std::cout << std::format("Device {}: {}", id, name) << std::endl;
		std::cout << std::format("\tInput channels: {}", inputChannels) << std::endl;
		std::cout << std::format("\tOutput channels: {}", outputChannels) << std::endl;
		std::cout << std::format("\tDefault input: {}", (isDefaultInput ? "yes" : "no")) << std::endl;
		std::cout << std::format("\tDefault output: {}", (isDefaultOutput ? "yes" : "no")) << std::endl;
		std::cout << std::endl;
	}

	deleteDeviceInformation(devices);
}
