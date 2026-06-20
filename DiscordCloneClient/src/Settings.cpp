#include "Settings.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>

#ifdef __LINUX__
#include <cstdlib>
#else
#include <ShlObj.h>
#endif

#include <JsonParser.h>

static constexpr std::string_view reconnectIpKey = "reconnectIp";
static constexpr std::string_view reconnectPortKey = "reconnectPort";
static constexpr std::string_view inputVolumeKey = "inputVolume";
static constexpr std::string_view outputVolumeKey = "outputVolume";

namespace client
{
	std::filesystem::path Settings::getPathToSettings()
	{
		std::filesystem::path result;

#ifdef __LINUX__
		
		if (const char* home = std::getenv("HOME"))
		{
			result = home;
		}
		else
		{
			throw std::runtime_error("Can't get HOME variable");
		}
#else
		PWSTR path = nullptr;
		HRESULT resultCode = SHGetKnownFolderPath
		(
			FOLDERID_RoamingAppData,
			0,
			nullptr,
			&path
		);

		if (!SUCCEEDED(resultCode))
		{
			throw std::runtime_error(std::format("Can't get path to {}", Settings::settingsFile));
		}

		result = path;

		CoTaskMemFree(path);		
#endif
		result /= PROJECT_NAME;

		if (!std::filesystem::exists(result))
		{
			std::filesystem::create_directories(result);
		}

		result /= Settings::settingsFile;

		return result;
	}

	std::optional<Settings> Settings::loadSettings()
	{
		std::filesystem::path settingsPath = Settings::getPathToSettings();

		if (!std::filesystem::exists(settingsPath))
		{
			return std::nullopt;
		}

		Settings result;
		std::ifstream stream(settingsPath);

		stream >> result;

		return result;
	}

	Settings::Settings() :
		reconnectPort(8080),
		inputVolume(1.0),
		outputVolume(1.0)
	{

	}

	void Settings::saveSettings() const
	{
		std::ofstream stream(Settings::getPathToSettings());

		stream << *this;
	}

	void Settings::modifySettings(const std::function<void(Settings&)>& modifyFunctor)
	{
		modifyFunctor(*this);

		this->saveSettings();
	}

	json::JsonObject Settings::operator *() const
	{
		json::JsonObject result;

		result[reconnectIpKey] = reconnectIp;
		result[reconnectPortKey] = reconnectPort;
		result[inputVolumeKey] = inputVolume;
		result[outputVolumeKey] = outputVolume;

		return result;
	}

	std::istream& operator >>(std::istream& stream, Settings& settings)
	{
		json::JsonParser parser(stream);

		parser.tryGet<std::string>(reconnectIpKey, settings.reconnectIp);
		parser.tryGet<uint16_t>(reconnectPortKey, settings.reconnectPort);
		parser.tryGet<double>(inputVolumeKey, settings.inputVolume);
		parser.tryGet<double>(outputVolumeKey, settings.outputVolume);

		return stream;
	}

	std::ostream& operator <<(std::ostream& stream, const Settings& settings)
	{
		return stream << *settings;
	}
}
