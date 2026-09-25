#pragma once

#include <Executors/StatelessExecutor.hpp>

#include <unordered_map>
#include <mutex>
#include <random>

#include "VoiceServer.hpp"

namespace executors
{
	class Room : public framework::StatelessExecutor
	{
	private:
		struct RoomData
		{
			std::string name;
			std::string password;

			bool operator ==(const RoomData& other) const noexcept;
		};

		struct RoomHash
		{
			size_t operator ()(const RoomData& roomData) const noexcept;
		};

	private:
		std::unordered_map<RoomData, server::VoiceServer, RoomHash> rooms; // TODO: resize may reinitialize servers
		std::mutex roomsMutex;
		std::mt19937_64 random;

	private:
		framework::JsonObject doGetVoice(framework::HttpRequest& request, framework::HttpResponse& response, decltype(rooms)::iterator it);

		framework::JsonObject doGetVideo(framework::HttpRequest& request, framework::HttpResponse& response, decltype(rooms)::iterator it);

		void doPostVoice(framework::HttpRequest& request, framework::HttpResponse& response, framework::JsonBuilder& builder, decltype(rooms)::iterator it);

		void doPostVideo(framework::HttpRequest& request, framework::HttpResponse& response, framework::JsonBuilder& builder, decltype(rooms)::iterator it);

	public:
		void init(const framework::utility::ExecutorSettings& settings) override;

		void doGet(framework::HttpRequest& request, framework::HttpResponse& response) override;

		void doPost(framework::HttpRequest& request, framework::HttpResponse& response) override;

		void doDelete(framework::HttpRequest& request, framework::HttpResponse& response) override;

		void doPatch(framework::HttpRequest& request, framework::HttpResponse& response) override;
	};
}
