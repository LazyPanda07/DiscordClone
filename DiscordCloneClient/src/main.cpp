#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <ranges>

#include <UDPSocket.hpp>
#include <c_api.h>

#include "Hotkeys.hpp"
#include "Settings.hpp"
#include "Wrappers/MicrophoneWrapper.hpp"
#include "Wrappers/SpeakerWrapper.hpp"
#include "Utils.hpp"
#include "Checks/CheckSocket.hpp"
#include "Checks/CheckMicrophone.hpp"
#include "Checks/CheckSpeaker.hpp"
#include "Commands/Connect.hpp"
#include "Commands/OverrideInputDeviceId.hpp"
#include "Commands/OverrideOutputDeviceId.hpp"
#include "Commands/SetMicrophoneVolume.hpp"
#include "Commands/SetSpeakerVolume.hpp"
#include "Commands/GetMicrophoneVolume.hpp"
#include "Commands/GetSpeakerVolume.hpp"
#include "Commands/MuteOrUnmute.hpp"
#include "Commands/Echo.hpp"
#include "Commands/Exit.hpp"

#ifndef __LINUX__
#include <Windows.h>
#endif

void restoreVolume(const client::Settings& settings, const std::unique_ptr<wrappers::MicrophoneWrapper>& input, const std::unique_ptr<wrappers::SpeakerWrapper>& output);

void printDeviceInfo(const std::unique_ptr<wrappers::MicrophoneWrapper>& input);

void help(const std::vector<std::unique_ptr<commands::Command>>& commands);

int main(int argc, char** argv) try
{
#ifndef __LINUX__
	SetConsoleOutputCP(CP_UTF8);
#endif

	client::Settings settings;
	std::unique_ptr<wrappers::SocketWrapper> socket;
	std::unique_ptr<wrappers::MicrophoneWrapper> input;
	std::unique_ptr<wrappers::SpeakerWrapper> output;
	functionality::Hotkeys hotkeys;
	std::vector<std::unique_ptr<checks::Check>> checks = [&socket, &input, &output]()
		{
			std::vector<std::unique_ptr<checks::Check>> result;

			result.emplace_back(std::make_unique<checks::CheckSocket>(socket));
			result.emplace_back(std::make_unique<checks::CheckMicrophone>(input));
			result.emplace_back(std::make_unique<checks::CheckSpeaker>(output));

			return result;
		}();
	std::vector<std::unique_ptr<commands::Command>> commands = [&socket, &input, &output, &settings, &checks]()
		{
			std::vector<std::unique_ptr<commands::Command>> result;

			result.emplace_back
			(
				std::make_unique<commands::Connect>
				(
					socket,
					settings,
					[&socket, &input, &output]()
					{
						input = std::make_unique<wrappers::MicrophoneWrapper>(*socket);
						output = std::make_unique<wrappers::SpeakerWrapper>(*socket);
					},
					checks
				)
			);
			result.emplace_back(std::make_unique<commands::SetMicrophoneVolume>(input, settings, checks));
			result.emplace_back(std::make_unique<commands::SetSpeakerVolume>(output, settings, checks));
			result.emplace_back(std::make_unique<commands::GetMicrophoneVolume>(input, checks));
			result.emplace_back(std::make_unique<commands::GetSpeakerVolume>(output, checks));
			result.emplace_back(std::make_unique<commands::MuteOrUnmute>(input, checks));
			result.emplace_back(std::make_unique<commands::Echo>(socket, checks));
			result.emplace_back(std::make_unique<commands::OverrideInputDeviceId>(input, checks));
			result.emplace_back(std::make_unique<commands::OverrideOutputDeviceId>(output, checks));
			result.emplace_back(std::make_unique<commands::Exit>(checks));

			return result;
		}();

	utils::callApiFunction(&initialize);

	if (std::optional<client::Settings> loadedSettings = client::Settings::loadSettings())
	{
		settings = *loadedSettings;

		auto it = std::ranges::find(commands, "connect", &commands::Command::command);

		if (it == commands.end())
		{
			throw std::runtime_error("Can't find connect command");
		}

		if (settings.reconnectIp.size())
		{
			std::istringstream stream(std::format("{}:{}", loadedSettings->reconnectIp, loadedSettings->reconnectPort));

			if (!(*it)->conditionalRun(stream))
			{
				settings.modifySettings
				(
					[](client::Settings& self)
					{
						self.reconnectIp = "";
						self.reconnectPort = 8080;
					}
				);
			}
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

			std::cout << "Enter command: ";
		},
		MOD_CONTROL | MOD_ALT,
		VK_SPACE
	);
#endif

	restoreVolume(settings, input, output);

	while (true)
	{
		std::string inputCommand;

		std::cout << "Enter command: ";

		std::getline(std::cin, inputCommand);

		std::istringstream is(inputCommand);
		std::string command;

		is >> command;

		if (command == "help")
		{
			help(commands);

			continue;
		}

		auto it = std::ranges::find(commands, command, &commands::Command::command);

		if (it == commands.end())
		{
			std::cout << "\033[31m" << "Wrong command: " << command << "\033[0m" << std::endl;

			help(commands);

			continue;
		}

		(*it)->conditionalRun(is);
	}

	return 0;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;

#ifndef __LINUX__
	std::cout << "Press any button to continue" << std::endl;

	std::cin.get();
#endif

	return 1;
}

void restoreVolume(const client::Settings& settings, const std::unique_ptr<wrappers::MicrophoneWrapper>& input, const std::unique_ptr<wrappers::SpeakerWrapper>& output)
{
	if (!input || !output)
	{
		return;
	}

	input->setVolume(settings.inputVolume);
	output->setVolume(settings.outputVolume);
}

void printDeviceInfo(const std::unique_ptr<wrappers::MicrophoneWrapper>& input)
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

void help(const std::vector<std::unique_ptr<commands::Command>>& commands)
{
	std::cout << "Available commands:" << std::endl;
	
	for (const std::unique_ptr<commands::Command>& command : commands)
	{
		std::cout << command->command << ": " << command->getHelpText() << std::endl;
	}

	std::cout << "help: " << std::endl;
}
