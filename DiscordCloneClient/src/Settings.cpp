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
static constexpr std::string_view roomNameKey = "roomName";
static constexpr std::string_view roomPasswordKey = "roomPassword";
static constexpr std::string_view microhponeVolumeKey = "microphoneVolume";
static constexpr std::string_view speakerVolumeKey = "speakerVolume";

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
		microphoneVolume(1.0),
		speakerVolume(1.0)
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
		result[microhponeVolumeKey] = microphoneVolume;
		result[speakerVolumeKey] = speakerVolume;
		result[roomNameKey] = roomName;
		result[roomPasswordKey] = roomPassword;

		return result;
	}

	std::istream& operator >>(std::istream& stream, Settings& settings)
	{
		json::JsonParser parser(stream);

		parser.tryGet<std::string>(reconnectIpKey, settings.reconnectIp);
		parser.tryGet<uint16_t>(reconnectPortKey, settings.reconnectPort);
		parser.tryGet<std::string>(roomNameKey, settings.roomName);
		parser.tryGet<std::string>(roomPasswordKey, settings.roomPassword);
		parser.tryGet<double>(microhponeVolumeKey, settings.microphoneVolume);
		parser.tryGet<double>(speakerVolumeKey, settings.speakerVolume);

		return stream;
	}

	std::ostream& operator <<(std::ostream& stream, const Settings& settings)
	{
		return stream << *settings;
	}
}
