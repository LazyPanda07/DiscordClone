#pragma once

#include <string>
#include <ostream>
#include <istream>
#include <optional>

#include <JsonObject.h>

namespace client
{
	struct Settings
	{
	public:
		static inline constexpr std::string_view settingsFile = "settings.json";

	public:
		static std::optional<Settings> loadSettings();

	public:
		std::string reconnectIp;
		std::string reconnectPort;
		double inputVolume;
		double outputVolume;

	public:
		Settings();

		void saveSettings() const;

		json::JsonObject operator *() const;

		~Settings() = default;
	};

	std::istream& operator >>(std::istream& stream, Settings& settings);

	std::ostream& operator <<(std::ostream& stream, const Settings& settings);
}
