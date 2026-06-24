#pragma once

#include <string>
#include <ostream>
#include <istream>
#include <optional>
#include <filesystem>
#include <functional>

#include <JsonObject.h>

namespace client
{
	struct Settings
	{
	public:
		static inline constexpr std::string_view settingsFile = "settings.json";

	public:
		static std::filesystem::path getPathToSettings();

		static std::optional<Settings> loadSettings();

	public:
		std::string reconnectIp;
		uint16_t reconnectPort;
		double microphoneVolume;
		double speakerVolume;

	private:
		void saveSettings() const;

	public:
		Settings();

		void modifySettings(const std::function<void(Settings&)>& modifyFunctor);

		json::JsonObject operator *() const;

		~Settings() = default;
	};

	std::istream& operator >>(std::istream& stream, Settings& settings);

	std::ostream& operator <<(std::ostream& stream, const Settings& settings);
}
