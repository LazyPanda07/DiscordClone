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

	void Room::doGet(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		const framework::JsonParser& data = request.getJson();
		RoomData roomData =
		{
			.name = data.get<std::string>("roomName"),
			.password = data.get<std::string>("roomPassword")
		};

		std::lock_guard<std::mutex> lock(roomsMutex);

		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (it->first.password == roomData.password)
			{
				runServer(it->second);

				response.setBody(std::to_string(it->second.getPort()));
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

			runServer(value->second);

			response.setResponseCode(framework::ResponseCodes::created);
			response.setBody(std::to_string(value->second.getPort()));
		}
	}

	DEFINE_EXECUTOR(Room)
}

void runServer(voice::VoiceServer& server)
{
	std::thread(&voice::VoiceServer::start, &server).detach();
}
