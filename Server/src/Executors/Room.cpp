#include "Executors/Room.hpp"

static void runServer(voice::VoiceServer& server);

namespace executors
{
	bool Room::RoomData::operator ==(const RoomData& other) const noexcept
	{
		return name == other.name;
	}

	size_t Room::RoomHash::operator ()(const RoomData& roomData) const noexcept
	{
		return std::hash<std::string>()(roomData.name);
	}

	void Room::init(const framework::utility::ExecutorSettings& settings)
	{
		random.seed(std::time(nullptr));
	}

	void Room::doGet(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		const std::unordered_map<std::string, std::string>& queryParameters = request.getQueryParameters();
		RoomData roomData =
		{
			.name = queryParameters.at("roomName"),
			.password = queryParameters.at("roomPassword")
		};

		std::lock_guard<std::mutex> lock(roomsMutex);

		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (it->first.password == roomData.password)
			{
				framework::JsonObject result;
				std::vector<std::string> clients = it->second.getClients();

				for (std::string& client : clients)
				{
					result.emplace_back(std::move(client));
				}

				response.setBody(result);
			}
			else
			{
				constexpr std::string_view errorMessage = "Wrong password";

				response.setResponseCode(framework::ResponseCodes::forbidden);
				response.setBody(errorMessage);
			}
		}
	}

	void Room::doPost(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		const framework::JsonParser& data = request.getJson();
		RoomData roomData =
		{
			.name = data.get<std::string>("roomName"),
			.password = data.get<std::string>("roomPassword")
		};

		framework::JsonBuilder builder;
		std::string userName = data.get<std::string>("userName");
		std::lock_guard<std::mutex> lock(roomsMutex);
		uint64_t id = random();

		builder["id"] = id;

		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (it->first.password == roomData.password)
			{
				it->second.addPendingClient(id, std::move(userName));

				runServer(it->second);

				builder["port"] = it->second.getPort();

				response.setBody(builder);
			}
			else
			{
				constexpr std::string_view errorMessage = "Wrong password";

				response.setResponseCode(framework::ResponseCodes::forbidden);
				response.setBody(errorMessage);
			}
		}
		else
		{
			const auto& [value, _] = rooms.try_emplace(std::move(roomData));

			value->second.addPendingClient(id, std::move(userName));

			runServer(value->second);

			builder["port"] = value->second.getPort();

			response.setResponseCode(framework::ResponseCodes::created);
			response.setBody(builder);
		}
	}

	DEFINE_EXECUTOR(Room)
}

void runServer(voice::VoiceServer& server)
{
	std::thread(&voice::VoiceServer::start, &server).detach();
}
