#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#include <UDPClientSocket.hpp>
#include <InputVoice.hpp>
#include <OutputVoice.hpp>
#include <Functionality.hpp>

#include "Hotkeys.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

void diagnoseAudioDevices();

int main(int argc, char** argv) try
{
	constexpr uint32_t sampleRate = 44100;
	constexpr uint32_t bufferFrames = 256;

#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif

	std::string command;
	std::unique_ptr<web::UDPSocket> socket;
	std::vector<std::pair<std::string, std::string>> availableCommands =
	{
		{ "send", "<message>" },
		{ "connect", "<ip:port>" },
		{ "mute", "" },
		{ "help", "" },
		{ "exit", "" }
	};

	std::unique_ptr<voice::InputVoice> input;
	std::unique_ptr<voice::OutputVoice> output;
	std::unique_ptr<functionality::Hotkeys> hotkeys;

	diagnoseAudioDevices();

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

			socket->sendData(message);
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

			socket = std::make_unique<web::UDPClientSocket>(ip.data(), static_cast<uint16_t>(std::stoi(port)));

			std::cout << "Connected to: " << ip << ':' << port << std::endl;

			input = std::make_unique<voice::InputVoice>(*socket, bufferFrames, sampleRate);
			output = std::make_unique<voice::OutputVoice>(*socket, bufferFrames, sampleRate);
			hotkeys = std::make_unique<functionality::Hotkeys>(*input, *output);

#if _WIN32
			hotkeys->registerHotkey
			(
				[](voice::InputVoice& inputVoice, voice::OutputVoice& outputVoice)
				{
					functionality::muteOrUnmute(inputVoice);
				},
				MOD_CONTROL | MOD_ALT,
				VK_SPACE
			);
#endif
		}
		else if (!command.find("mute"))
		{
			functionality::muteOrUnmute(*input);
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

void diagnoseAudioDevices()
{
	std::vector<RtAudio::DeviceInfo> devices = functionality::getAudioDevices();

	std::cout << std::format("Found {} audio devices:", devices.size()) << std::endl << std::endl;

	for (const RtAudio::DeviceInfo& info : devices)
	{
		std::cout << "Device " << info.ID << ": " << info.name << std::endl;
		std::cout << "  Input channels: " << info.inputChannels << std::endl;
		std::cout << "  Output channels: " << info.outputChannels << std::endl;
		std::cout << "  Default input: " << (info.isDefaultInput ? "yes" : "no") << std::endl;
		std::cout << "  Default output: " << (info.isDefaultOutput ? "yes" : "no") << std::endl;
		std::cout << std::endl;
	}
}
