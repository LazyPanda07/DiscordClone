#include <iostream>
#include <filesystem>

#include <ConsoleArgumentParser.h>

#ifdef __LINUX__
#include <signal.h>
#else
#include <Windows.h>
#endif

#include "Releases.hpp"

constexpr std::string_view discordCloneExecutableName =
#ifdef __LINUX__
"DiscordClone";
#else
"DiscordClone.exe";
#endif

std::vector<std::filesystem::path> cleanFiles;

int main(int argc, char** argv) try
{
	std::filesystem::path applicationDirectory = std::filesystem::absolute(std::filesystem::path(argv[0]));

	applicationDirectory = applicationDirectory.parent_path();

	utility::parsers::ConsoleArgumentParser arguments(argc, argv);

	int32_t processId = arguments.get<uint32_t>("process_id", -1);
	bool startDiscordClone = arguments.get<bool>("start_discord_clone", false);
	int32_t majorVersion = arguments.getRequired<int32_t>("major_version");
	int32_t minorVersion = arguments.getRequired<int32_t>("minor_version");
	int32_t patch = arguments.getRequired<int32_t>("patch");

	int32_t latestMajorVersion;
	int32_t latestMinorVersion;
	int32_t latestPatch;
	std::vector<json::JsonObject> assets;

	if (processId != -1)
	{
#ifdef __LINUX__
		kill(static_cast<pid_t>(processId), SIGTERM);
#else
		HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, processId);

		TerminateProcess(process, 0);

		CloseHandle(process);
#endif
	}

	releases::getLatestVersion(latestMajorVersion, latestMinorVersion, latestPatch, assets);

	if (majorVersion != latestMajorVersion || minorVersion != latestMinorVersion || patch != latestPatch)
	{
		constexpr std::string_view github = "https://github.com";
		std::vector<std::string> updateFiles =
		{
#ifdef __LINUX__
			"DiscordClone",
			"libDiscordCloneClientResources.so",
			"libDiscordCloneClientLibrary.so"
#else
			"DiscordClone.exe",
			"DiscordCloneClientResources.dll",
			"DiscordCloneClientLibrary.dll"
#endif
		};

		auto it = std::find_if(assets.begin(), assets.end(), [](const json::JsonObject& asset) { return asset["name"].get<std::string>() == releases::getAssetName(); });

		if (it == assets.end())
		{
			throw std::runtime_error(std::format("Can't find asset with name: {}", releases::getAssetName()));
		}

		const json::JsonObject& asset = *it;
		std::string_view downloadLink = asset["browser_download_url"].get<std::string>();
		
		std::filesystem::path zipPath = releases::get(std::string_view(downloadLink.data() + github.size()));
		cleanFiles.push_back(zipPath);

		std::filesystem::path zipContentPath = releases::unpack(zipPath);
		cleanFiles.push_back(zipContentPath);

		releases::replace(applicationDirectory, zipContentPath, updateFiles);

		releases::clean(cleanFiles);
	}

	if (startDiscordClone)
	{
		std::system(std::format("{} --skip_check_updates", (applicationDirectory / discordCloneExecutableName).string()).data());
	}

	return 0;
}
catch (const std::exception& e)
{
	releases::clean(cleanFiles);

	std::cerr << e.what() << std::endl;

	std::cin.get();

	return 1;
}
