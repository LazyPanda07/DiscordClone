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
		using namespace std::chrono_literals;

		const framework::JsonParser& data = request.getJson();
		RoomData roomData =
		{
			.name = data.get<std::string>("roomName"),
			.password = data.get<std::string>("roomPassword")
		};
		std::atomic_bool called(false);
		uint16_t notificationServerPort;
		auto setter = [&notificationServerPort, &called](uint16_t port)
			{
				notificationServerPort = port;

				called = true;
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

				it->second.start(setter);

				while (!called)
				{
					std::this_thread::sleep_for(50ms);
				}

				builder["port"] = it->second.getPort();
				builder["notificationServerPort"] = notificationServerPort;

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

			value->second.addPendingClient(id, std::move(userName));

			value->second.start(setter);

			while (!called)
			{
				std::this_thread::sleep_for(50ms);
			}

			builder["port"] = value->second.getPort();
			builder["notificationServerPort"] = notificationServerPort;

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

	DEFINE_EXECUTOR(Room)
}
