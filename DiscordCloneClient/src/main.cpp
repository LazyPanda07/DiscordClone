#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

#include <UDPClientSocket.hpp>
#include <InputVoice.hpp>
#include <OutputVoice.hpp>
#include <Functionality.hpp>

#include <JsonParser.h>
#include <JsonBuilder.h>

#include "Hotkeys.hpp"

#ifndef __LINUX__
#include <Windows.h>
#endif

struct Settings
{
	std::string reconnectIp;
	std::string reconnectPort;
	double inputVolume = 1.0;
	double outputVolume = 1.0;
};

void printAudioDevicesInfo();

std::pair<std::string, std::string> connect(std::string& command, std::unique_ptr<web::UDPSocket>& resultSocket, std::unique_ptr<voice::InputVoice>& input, std::unique_ptr<voice::OutputVoice>& output, std::unique_ptr<functionality::Hotkeys>& hotkeys, const Settings& settings, std::optional<bool>& connected);

void connect(std::string_view ip, std::string_view port, std::unique_ptr<web::UDPSocket>& resultSocket, std::unique_ptr<voice::InputVoice>& input, std::unique_ptr<voice::OutputVoice>& output, std::unique_ptr<functionality::Hotkeys>& hotkeys, const Settings& settings, std::optional<bool>& connected);

void saveSettings(const Settings& settings);

std::optional<Settings> loadSettings();

int main(int argc, char** argv) try
{
#ifndef __LINUX__
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
		{ "echo", "" },
		{ "help", "" },
		{ "exit", "" }
	};

	printAudioDevicesInfo();

	Settings settings;

	if (std::optional<Settings> loadedSettings = loadSettings())
	{
		std::optional<bool> connected;

		connect(loadedSettings->reconnectIp, loadedSettings->reconnectPort, socket, input, output, hotkeys, *loadedSettings, connected);

		if (connected && *connected)
		{
			settings = *loadedSettings;
		}
	}

	std::optional<bool> connected;

	while (true)
	{
		std::cout << "Enter command: ";

		std::getline(std::cin, command);

		if (!command.find("connect"))
		{
			connected = std::nullopt;

			auto [ip, port] = connect(command, socket, input, output, hotkeys, settings, connected);

			if (connected)
			{
				settings.reconnectIp = ip;
				settings.reconnectPort = port;

				saveSettings(settings);
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
			input->restart();
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
			output->restart();
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

			saveSettings(settings);
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

			saveSettings(settings);
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

			functionality::muteOrUnmute(*input);
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

std::pair<std::string, std::string> connect(std::string& command, std::unique_ptr<web::UDPSocket>& resultSocket, std::unique_ptr<voice::InputVoice>& input, std::unique_ptr<voice::OutputVoice>& output, std::unique_ptr<functionality::Hotkeys>& hotkeys, const Settings& settings, std::optional<bool>& connected)
{
	std::istringstream is(command);
	std::string ip;
	std::string port;

	is >> command;

	std::getline(is, ip, ':');

	ip.erase(ip.begin());

	is >> port;

	connect(ip, port, resultSocket, input, output, hotkeys, settings, connected);

	return std::make_pair(std::move(ip), std::move(port));
}

void connect(std::string_view ip, std::string_view port, std::unique_ptr<web::UDPSocket>& resultSocket, std::unique_ptr<voice::InputVoice>& input, std::unique_ptr<voice::OutputVoice>& output, std::unique_ptr<functionality::Hotkeys>& hotkeys, const Settings& settings, std::optional<bool>& connected)
{
	static constexpr uint32_t sampleRate = 48000;
	static constexpr uint32_t bufferFrames = 256;

	resultSocket = std::make_unique<web::UDPClientSocket>(ip, static_cast<uint16_t>(std::stoi(port.data())));

	std::cout << std::format("Connecting to: {}:{}...", ip, port) << std::endl;

	resultSocket->sendData(web::UDPSocket::hello);

	std::this_thread::sleep_for(std::chrono::seconds(5));

	resultSocket->receiveData
	(
		[&input, &output, &hotkeys, &resultSocket, &settings, &connected, ip, port](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
		{
			if (size == web::UDPSocket::helloPacketSize && std::equal(data.begin(), data.begin() + size, web::UDPSocket::hello.begin()))
			{
				std::cout << "Connected to: " << ip << ':' << port << std::endl;

				input = std::make_unique<voice::InputVoice>(*resultSocket, bufferFrames, sampleRate);
				output = std::make_unique<voice::OutputVoice>(*resultSocket, bufferFrames, sampleRate);
				hotkeys = std::make_unique<functionality::Hotkeys>(*input, *output);

				input->setVolume(settings.inputVolume);
				output->setVolume(settings.outputVolume);

#ifndef __LINUX__
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
				connected = true;
			}
			else
			{
				std::cout << "Failed to connect to: " << ip << ':' << port << std::endl;

				connected = false;
			}
		}
	);
}

void saveSettings(const Settings& settings)
{
	constexpr std::string_view settingsFile = "settings.json";

	json::JsonBuilder builder;

	builder["reconnectIp"] = settings.reconnectIp;
	builder["reconnectPort"] = settings.reconnectPort;
	builder["inputVolume"] = settings.inputVolume;
	builder["outputVolume"] = settings.outputVolume;

	std::ofstream(settingsFile.data()) << builder;
}

std::optional<Settings> loadSettings()
{
	constexpr std::string_view settingsFile = "settings.json";

	if (!std::filesystem::exists(settingsFile))
	{
		return std::nullopt;
	}

	Settings settings;
	json::JsonParser parser(std::ifstream(settingsFile.data()));

	parser.tryGet<std::string>("reconnectIp", settings.reconnectIp);
	parser.tryGet<std::string>("reconnectPort", settings.reconnectPort);
	parser.tryGet<double>("inputVolume", settings.inputVolume);
	parser.tryGet<double>("outputVolume", settings.outputVolume);

	return settings;
}
