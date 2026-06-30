#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <ranges>

#include <UDPSocket.hpp>
#include <c_api.h>
#include <IOSocketStream.h>
#include <HttpBuilder.h>
#include <ConsoleArgumentParser.h>

#include "Hotkeys.hpp"
#include "Settings.hpp"
#include "Wrappers/MicrophoneWrapper.hpp"
#include "Wrappers/SpeakerWrapper.hpp"
#include "Utils.hpp"
#include "Checks/CheckSocket.hpp"
#include "Checks/CheckMicrophone.hpp"
#include "Checks/CheckSpeaker.hpp"
#include "Checks/CheckStream.hpp"
#include "Commands/Connect.hpp"
#include "Commands/OverrideMicrophoneDeviceId.hpp"
#include "Commands/OverrideSpeakerDeviceId.hpp"
#include "Commands/SetMicrophoneVolume.hpp"
#include "Commands/SetSpeakerVolume.hpp"
#include "Commands/GetMicrophoneVolume.hpp"
#include "Commands/GetSpeakerVolume.hpp"
#include "Commands/MuteOrUnmute.hpp"
#include "Commands/Echo.hpp"
#include "Commands/Exit.hpp"
#include "Commands/PingCommand.hpp"
#include "Commands/GetUsers.hpp"
#include "Commands/GetVersion.hpp"

#ifndef __LINUX__
#include <Windows.h>
#endif

std::unique_ptr<streams::IOSocketStream> controlStream;
std::unique_ptr<wrappers::MicrophoneWrapper> microphone;
client::Settings settings;
uint64_t id;

void restoreVolume(const client::Settings& settings, const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::unique_ptr<wrappers::SpeakerWrapper>& speaker);

void printDeviceInfo(const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone);

void help(const std::vector<std::unique_ptr<commands::Command>>& commands);

#ifdef __LINUX__

#else
BOOL onExit(DWORD CtrlType);
#endif

int main(int argc, char** argv) try
{
	std::filesystem::path applicationDirectory = std::filesystem::absolute(std::filesystem::path(argv[0]));

	applicationDirectory = applicationDirectory.parent_path();

#ifndef __LINUX__
	SetConsoleOutputCP(CP_UTF8);
#endif

	utility::parsers::ConsoleArgumentParser argumentParser(argc, argv);

	if (!argumentParser.get<bool>("skip_check_updates", false))
	{
		constexpr std::string_view discordCloneUpdaterExecutableName =
#ifdef __LINUX__
			"DiscordCloneUpdater";
#else
			"DiscordCloneUpdater.exe";
#endif

		int32_t major = 0;
		int32_t minor = 0;
		int32_t patch = 0;
		uint32_t processId = GetProcessId(GetCurrentProcess());

		utils::callApiFunction(&::getVersionExtended, &major, &minor, &patch);

		std::system(std::format("{} --process_id {} --start_discord_clone --major_version {} --minor_version {} --patch {}", (applicationDirectory / discordCloneUpdaterExecutableName).string(), processId, major, minor, patch).data());

		return 0;
	}

	std::unique_ptr<wrappers::SocketWrapper> socket;
	std::unique_ptr<wrappers::SpeakerWrapper> speaker;
	functionality::Hotkeys hotkeys;
	std::vector<std::unique_ptr<checks::Check>> checks = [&socket, &speaker]()
		{
			std::vector<std::unique_ptr<checks::Check>> result;

			result.emplace_back(std::make_unique<checks::CheckSocket>(socket));
			result.emplace_back(std::make_unique<checks::CheckMicrophone>(microphone));
			result.emplace_back(std::make_unique<checks::CheckSpeaker>(speaker));
			result.emplace_back(std::make_unique<checks::CheckStream>(controlStream));

			return result;
		}();
	std::vector<std::unique_ptr<commands::Command>> commands = [&socket, &speaker, &checks]()
		{
			std::vector<std::unique_ptr<commands::Command>> result;

			result.emplace_back
			(
				std::make_unique<commands::Connect>
				(
					socket,
					controlStream,
					settings,
					[&socket, &speaker](uint64_t& resultId)
					{
						id = resultId;

						microphone = std::make_unique<wrappers::MicrophoneWrapper>(*socket);
						speaker = std::make_unique<wrappers::SpeakerWrapper>(*socket);

						restoreVolume(settings, microphone, speaker);
					},
					checks
				)
			);
			result.emplace_back(std::make_unique<commands::SetMicrophoneVolume>(microphone, settings, checks));
			result.emplace_back(std::make_unique<commands::SetSpeakerVolume>(speaker, settings, checks));
			result.emplace_back(std::make_unique<commands::GetMicrophoneVolume>(microphone, checks));
			result.emplace_back(std::make_unique<commands::GetSpeakerVolume>(speaker, checks));
			result.emplace_back(std::make_unique<commands::MuteOrUnmute>(microphone, checks));
			result.emplace_back(std::make_unique<commands::Echo>(socket, checks));
			result.emplace_back(std::make_unique<commands::OverrideMicrophoneDeviceId>(microphone, checks));
			result.emplace_back(std::make_unique<commands::OverrideSpeakerDeviceId>(speaker, checks));
			result.emplace_back(std::make_unique<commands::Exit>(checks));
			result.emplace_back(std::make_unique<commands::PingCommand>(controlStream, checks));
			result.emplace_back(std::make_unique<commands::GetUsers>(controlStream, settings, checks));
			result.emplace_back(std::make_unique<commands::GetVersion>(checks));

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
			std::istringstream stream(std::format("{}:{} {} {} {}", loadedSettings->reconnectIp, loadedSettings->reconnectPort, loadedSettings->userName, loadedSettings->roomName, loadedSettings->roomPassword));

			if (!(*it)->conditionalRun(stream))
			{
				settings.modifySettings
				(
					[](client::Settings& self)
					{
						self.reconnectIp = "";
						self.reconnectPort = 8080;
						self.roomName = "";
						self.roomPassword = "";
					}
				);
			}
		}
	}

	printDeviceInfo(microphone);

#ifndef __LINUX__
	SetConsoleCtrlHandler(&onExit, TRUE);

	hotkeys.registerHotkey
	(
		[]()
		{
			microphone->muteOrUnmute();

			printDeviceInfo(microphone);

			std::cout << "Enter command: ";
		},
		MOD_CONTROL | MOD_ALT,
		VK_SPACE
	);
#endif

	while (true)
	{
		std::string microphoneCommand;

		std::cout << "Enter command: ";

		std::getline(std::cin, microphoneCommand);

		std::istringstream is(microphoneCommand);
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

void restoreVolume(const client::Settings& settings, const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone, const std::unique_ptr<wrappers::SpeakerWrapper>& speaker)
{
	if (!microphone || !speaker)
	{
		return;
	}

	microphone->setVolume(settings.microphoneVolume);
	speaker->setVolume(settings.speakerVolume);
}

void printDeviceInfo(const std::unique_ptr<wrappers::MicrophoneWrapper>& microphone)
{
	DeviceInformationArray devices = utils::callApiFunction(&getDeviceInformation);
	uint64_t size = utils::callApiFunction(&getDeviceInformationSize, devices);

	if (microphone)
	{
		std::cout << std::endl << std::format("Microphone state: {}", microphone->isStreamRunning() ? "🟢" : "🔴") << std::endl;
	}

	std::cout << std::format("Found {} audio devices:", size) << std::endl << std::endl;

	for (uint64_t i = 0; i < size; i++)
	{
		uint32_t id = utils::callApiFunction(&getDeviceInformationId, devices, i);
		std::string_view name = utils::callApiFunction(&getDeviceInformationName, devices, i);
		uint32_t microphoneChannels = utils::callApiFunction(&getDeviceInformationInputChannels, devices, i);
		uint32_t speakerChannels = utils::callApiFunction(&getDeviceInformationOutputChannels, devices, i);
		bool isDefaultInput = utils::callApiFunction(&getDeviceInformationDefaultInput, devices, i);
		bool isDefaultOutput = utils::callApiFunction(&getDeviceInformationDefaultOutput, devices, i);

		std::cout << std::format("Device {}: {}", id, name) << std::endl;
		std::cout << std::format("\tInput channels: {}", microphoneChannels) << std::endl;
		std::cout << std::format("\tOutput channels: {}", speakerChannels) << std::endl;
		std::cout << std::format("\tDefault microphone: {}", (isDefaultInput ? "yes" : "no")) << std::endl;
		std::cout << std::format("\tDefault speaker: {}", (isDefaultOutput ? "yes" : "no")) << std::endl;
		std::cout << std::endl;
	}

	deleteDeviceInformation(devices);
}

void help(const std::vector<std::unique_ptr<commands::Command>>& commands)
{
	std::cout << "Available commands:" << std::endl;
	
	for (const std::unique_ptr<commands::Command>& command : commands)
	{
		std::cout << std::format("{}: {}", command->command, command->getHelpText()) << std::endl;
	}

	std::cout << "help: " << std::endl;
}

#ifdef __LINUX__

#else
BOOL onExit(DWORD CtrlType)
{
	if (CtrlType == CTRL_CLOSE_EVENT || CtrlType == CTRL_SHUTDOWN_EVENT)
	{
		std::string request;
		std::string _;

		json::JsonBuilder builder;

		builder["roomName"] = settings.roomName;
		builder["roomPassword"] = settings.roomPassword;
		builder["id"] = id;

		request = web::HttpBuilder()
			.deleteRequest()
			.parameters("room")
			.build(builder);

		try
		{
			if (microphone)
			{
				microphone->sendSilence();
			}
		}
		catch (const std::exception&)
		{

		}

		try
		{
			if (controlStream)
			{
				(*controlStream) << request;
				(*controlStream) >> _;
			}
		}
		catch (const std::exception&)
		{

		}

		return TRUE;
	}

	return FALSE;
}
#endif
