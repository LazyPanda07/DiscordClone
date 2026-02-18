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

constexpr uint32_t sampleRate = 48000;
constexpr uint32_t bufferFrames = 256;

void printAudioDevicesInfo();

void connect(std::string& command, std::unique_ptr<web::UDPSocket>& socket, std::unique_ptr<voice::InputVoice>& input, std::unique_ptr<voice::OutputVoice>& output, std::unique_ptr<functionality::Hotkeys>& hotkeys);

int main(int argc, char** argv) try
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif

	std::string command;
	std::unique_ptr<web::UDPSocket> socket;
	std::unique_ptr<voice::InputVoice> input;
	std::unique_ptr<voice::OutputVoice> output;
	std::unique_ptr<functionality::Hotkeys> hotkeys;
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
		{ "help", "" },
		{ "exit", "" }
	};

	printAudioDevicesInfo();

	while (true)
	{
		std::cout << "Enter command: ";

		std::getline(std::cin, command);

		if (!command.find("connect"))
		{
			connect(command, socket, input, output, hotkeys);
		}
		else if (!command.find("override_input_device_id"))
		{
			std::istringstream is(command);
			uint32_t id;

			is >> command;
			is >> id;

			input->overrideDeviceId(id);
			input->restart();
		}
		else if (!command.find("override_output_device_id"))
		{
			std::istringstream is(command);
			uint32_t id;

			is >> command;
			is >> id;

			output->overrideDeviceId(id);
			output->restart();
		}
		else if (!command.find("set_input_volume"))
		{
			std::istringstream is(command);
			double volume;

			is >> command;
			is >> volume;

			input->setVolume(volume);
		}
		else if (!command.find("set_output_volume"))
		{
			std::istringstream is(command);
			double volume;

			is >> command;
			is >> volume;

			output->setVolume(volume);
		}
		else if (!command.find("get_input_volume"))
		{
			std::cout << input->getVolume() << std::endl;
		}
		else if (!command.find("get_output_volume"))
		{
			std::cout << output->getVolume() << std::endl;
		}
		else if (!command.find("mute_and_unmute"))
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
			exit(0);
		}
	}

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

	return 1;
}

void printAudioDevicesInfo()
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

void connect(std::string& command, std::unique_ptr<web::UDPSocket>& socket, std::unique_ptr<voice::InputVoice>& input, std::unique_ptr<voice::OutputVoice>& output, std::unique_ptr<functionality::Hotkeys>& hotkeys)
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
