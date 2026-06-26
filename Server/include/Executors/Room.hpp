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
		std::unordered_map<RoomData, voice::VoiceServer, RoomHash> rooms; // TODO: resize may reinitialize servers
		std::mutex roomsMutex;
		std::mt19937_64 random;

	public:
		void init(const framework::utility::ExecutorSettings& settings) override;

		void doGet(framework::HttpRequest& request, framework::HttpResponse& response) override;
	};
}
