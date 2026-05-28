#include "Settings.hpp"

#include <filesystem>
#include <fstream>

#include <JsonParser.h>

static constexpr std::string_view reconnectIpKey = "reconnectIp";
static constexpr std::string_view reconnectPortKey = "reconnectPort";
static constexpr std::string_view inputVolumeKey = "inputVolume";
static constexpr std::string_view outputVolumeKey = "outputVolume";

namespace client
{
	std::optional<Settings> Settings::loadSettings()
	{
		if (!std::filesystem::exists(Settings::settingsFile))
		{
			return std::nullopt;
		}

		Settings result;
		std::ifstream stream(Settings::settingsFile.data());

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
		

		std::ofstream stream(Settings::settingsFile.data());

		stream << *this;
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
