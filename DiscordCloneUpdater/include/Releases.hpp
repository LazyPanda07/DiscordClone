#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include <JsonObject.h>

namespace releases
{
	void getLatestVersion(int32_t& major, int32_t& minor, int32_t& patch, std::vector<json::JsonObject>& assets);

	std::filesystem::path get(std::string_view url);

	std::filesystem::path unpack(const std::filesystem::path& zipPath);

	void replace(const std::filesystem::path& applicationDirectory, const std::filesystem::path& zipContentPath, const std::vector<std::string>& files);

	void clean(const std::vector<std::filesystem::path>& paths);

	std::string_view getAssetName();
}
