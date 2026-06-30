#include "Releases.hpp"

#include <fstream>

#include <Http/HttpsNetwork.h>
#include <IOSocketStream.h>
#include <HttpBuilder.h>
#include <HttpParser.h>
#include <PatternParser.h>
#include <miniz.h>
#include <UUID.h>

constexpr std::string_view owner = "LazyPanda07";
constexpr std::string_view repository = "DiscordClone";

constexpr std::string_view userAgent = "DiscordCloneUpdater";
constexpr std::string_view defaultHost = "api.github.com";
constexpr std::string_view githubHost = "github.com";

template<>
struct utility::parsers::Converter<int32_t>
{
	void convert(std::string_view data, int32_t& result)
	{
		result = std::stoi(data.data());
	}
};

static streams::IOSocketStream getStream(std::string_view host = defaultHost);

namespace releases
{
	void getLatestVersion(int32_t& major, int32_t& minor, int32_t& patch, std::vector<json::JsonObject>& assets)
	{
		constexpr utility::parsers::PatternParser<int32_t, int32_t, int32_t> patternParser("v{}.{}.{}");

		std::string request;
		std::string response;
		streams::IOSocketStream stream = getStream();
		
		request = web::HttpBuilder()
			.getRequest()
			.parameters(std::format("repos/{}/{}/releases/latest", owner, repository))
			.headers
			(
				"User-Agent", userAgent,
				"Host", defaultHost
			)
			.build();

		stream << request;
		stream >> response;

		web::HttpParser parser(response);

		if (parser.getResponseCode() >= 300)
		{
			throw std::runtime_error(parser.getBody());
		}

		const json::JsonParser& jsonData = parser.getJson();
		std::string_view tagName = jsonData.get<std::string>("tag_name");

		patternParser.parse(tagName, major, minor, patch);

		assets = jsonData.get<std::vector<json::JsonObject>>("assets");
	}

	std::filesystem::path get(std::string_view url)
	{
		std::string request;
		std::string response;
		streams::IOSocketStream stream = getStream(githubHost);

		std::cout << std::format("Downloading https://{}{}...", url, githubHost) << std::endl;

		request = web::HttpBuilder()
			.getRequest()
			.parameters(url)
			.headers
			(
				"User-Agent", userAgent,
				"Host", githubHost
			)
			.build();

		stream << request;
		stream >> response;

		web::HttpParser parser(response);

		if (parser.getResponseCode() == web::ResponseCodes::found)
		{
			constexpr std::string_view releaseAssetsHost = "release-assets.githubusercontent.com";
			constexpr std::string_view protocolAndHost = "https://release-assets.githubusercontent.com";

			stream = getStream(releaseAssetsHost);

			request = web::HttpBuilder()
				.getRequest()
				.parameters(std::string_view(parser.getHeaders().at("Location").data() + protocolAndHost.size()))
				.headers
				(
					"User-Agent", userAgent,
					"Host", releaseAssetsHost
				)
				.build();

			stream << request;
			stream >> response;

			std::filesystem::path result = std::filesystem::temp_directory_path() / getAssetName();

			std::ofstream(result, std::ios::binary).write(response.data(), response.size());

			return result;
		}
		else
		{
			throw std::runtime_error(parser.getBody());
		}
	}

	std::filesystem::path unpack(const std::filesystem::path& zipPath)
	{
		std::string uuid = utility::uuid::generateUUID();
		std::filesystem::path zipContentPath = std::filesystem::temp_directory_path() / uuid;
		mz_zip_archive zip{};

		std::cout << std::format("Creating directory: {}", zipContentPath.string()) << std::endl;

		std::filesystem::create_directories(zipContentPath);

		try
		{
			if (!mz_zip_reader_init_file(&zip, zipPath.string().data(), 0))
			{
				throw std::runtime_error(mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
			}

			mz_uint files = mz_zip_reader_get_num_files(&zip);

			std::cout << std::format("Extracting {} files", files) << std::endl;
			
			for (mz_uint i = 0; i < files; i++)
			{
				mz_zip_archive_file_stat stat{};

				if (!mz_zip_reader_file_stat(&zip, i, &stat))
				{
					throw std::runtime_error(mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
				}

				std::filesystem::path fileName = stat.m_filename;

				fileName = fileName.filename();

				std::cout << std::format("Extracting {}...", fileName.string()) << std::endl;

				if (!mz_zip_reader_extract_to_file(&zip, stat.m_file_index, (zipContentPath / fileName).string().data(), 0))
				{
					throw std::runtime_error(mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
				}
			}

			mz_zip_reader_end(&zip);
		}
		catch (const std::exception&)
		{
			mz_zip_reader_end(&zip);

			throw;
		}

		return zipContentPath;
	}

	void replace(const std::filesystem::path& applicationDirectory, const std::filesystem::path& zipContentPath, const std::vector<std::string>& files)
	{
		for (const auto& recursiveIterator : std::filesystem::recursive_directory_iterator(zipContentPath))
		{
			if (auto it = std::find(files.begin(), files.end(), recursiveIterator.path().filename().string()); it != files.end())
			{
				std::cout << std::format("Copy {} to {}", recursiveIterator.path().string(), applicationDirectory.string()) << std::endl;

				std::filesystem::copy_file(recursiveIterator, applicationDirectory / *it, std::filesystem::copy_options::overwrite_existing);
			}
		}
	}

	void clean(const std::vector<std::filesystem::path>& paths)
	{
		for (const std::filesystem::path& path : paths)
		{
			std::cout << std::format("Cleaning {}", path.string()) << std::endl;

			std::filesystem::remove_all(path);
		}
	}

	std::string_view getAssetName()
	{
		constexpr std::string_view assetName =
#ifdef __LINUX__
			"linux.zip";
#else
			"windows.zip";
#endif

		return assetName;
	}
}

streams::IOSocketStream getStream(std::string_view host)
{
	return streams::IOSocketStream::createStream<web::http::HttpsNetwork>(host);
}
