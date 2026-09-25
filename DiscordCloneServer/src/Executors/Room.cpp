#include "Executors/Room.hpp"

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

	framework::JsonObject Room::doGetVoice(framework::HttpRequest& request, framework::HttpResponse& response, decltype(rooms)::iterator it)
	{
		framework::JsonObject result;
		std::vector<std::string> clients = it->second.getClients();

		for (std::string& client : clients)
		{
			result.emplace_back(std::move(client));
		}

		return result;
	}

	framework::JsonObject Room::doGetVideo(framework::HttpRequest& request, framework::HttpResponse& response, decltype(rooms)::iterator it)
	{
		return framework::JsonObject();
	}

	void Room::doPostVoice(framework::HttpRequest& request, framework::HttpResponse& response, framework::JsonBuilder& builder, decltype(rooms)::iterator it)
	{
		using namespace std::chrono_literals;

		std::atomic_bool called(false);
		uint16_t notificationServerPort;
		auto setter = [&notificationServerPort, &called](uint16_t port)
			{
				notificationServerPort = port;

				called = true;
			};
		uint64_t id = random();

		builder["id"] = id;

		it->second.addPendingClient(id, std::move(request.getJson().get<std::string>("userName")));

		it->second.start(setter);

		while (!called)
		{
			std::this_thread::sleep_for(50ms);
		}

		builder["port"] = it->second.getPort();
		builder["notificationPort"] = notificationServerPort;
	}

	void Room::doPostVideo(framework::HttpRequest& request, framework::HttpResponse& response, framework::JsonBuilder& builder, decltype(rooms)::iterator it)
	{

	}

	void Room::doGet(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		const std::unordered_map<std::string, std::string>& queryParameters = request.getQueryParameters();
		RoomData roomData =
		{
			.name = queryParameters.at("roomName"),
			.password = queryParameters.at("roomPassword")
		};
		std::string streamType = request.getRouteParameter<std::string>("streamType");

		std::lock_guard<std::mutex> lock(roomsMutex);

		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (it->first.password == roomData.password)
			{
				framework::JsonObject result;

				if (streamType == "voice")
				{
					result = this->doGetVoice(request, response, it);
				}
				else if (streamType == "video")
				{
					result = this->doGetVideo(request, response, it);
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
		std::string streamType = request.getRouteParameter<std::string>("streamType");
		std::lock_guard<std::mutex> lock(roomsMutex);
		
		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (it->first.password == roomData.password)
			{
				if (streamType == "voice")
				{
					this->doPostVoice(request, response, builder, it);
				}
				else if (streamType == "video")
				{
					this->doPostVideo(request, response, builder, it);
				}

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
			const auto& [value, _] = rooms.try_emplace(std::move(roomData), request.getServerIpV4());

			if (streamType == "voice")
			{
				this->doPostVoice(request, response, builder, value);
			}
			else if (streamType == "video")
			{
				this->doPostVideo(request, response, builder, value);
			}

			response.setResponseCode(framework::ResponseCodes::created);
			response.setBody(builder);
		}
	}

	void Room::doDelete(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		const framework::JsonParser& data = request.getJson();
		RoomData roomData =
		{
			.name = data.get<std::string>("roomName"),
			.password = data.get<std::string>("roomPassword")
		};
		uint64_t id = data.get<uint64_t>("id");

		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (it->first.password == roomData.password)
			{
				it->second.removeClient(id);
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
			constexpr std::string_view errorMessage = "Wrong room";

			response.setResponseCode(framework::ResponseCodes::forbidden);
			response.setBody(errorMessage);
		}
	}

	void Room::doPatch(framework::HttpRequest& request, framework::HttpResponse& response)
	{
		const framework::JsonParser& data = request.getJson();
		const std::unordered_map<std::string, std::string>& queryParameters = request.getQueryParameters();
		RoomData roomData =
		{
			.name = queryParameters.at("roomName"),
			.password = queryParameters.at("roomPassword")
		};
		uint64_t id = data.get<uint64_t>("id");

		std::lock_guard<std::mutex> lock(roomsMutex);

		if (auto it = rooms.find(roomData); it != rooms.end())
		{
			if (!it->second.isRunning())
			{
				return;
			}

			server::NotificationsServer& server = it->second.getNotificationServer();

			server.pushNotification(id, "fix_speaker_delay");
		}
	}

	DEFINE_EXECUTOR(Room)
}
