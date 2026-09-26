#pragma once

#include "Command.hpp"

#include <thread>

#include <IOSocketStream.h>

#include "Wrappers/SocketWrapper.hpp"
#include "Settings.hpp"

namespace commands
{
	class StartStream : public Command
	{
	private:
		std::unique_ptr<streams::IOSocketStream>& controlStream;
		std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket;
		client::Settings& settings;
		std::jthread& streamThread;
		uint64_t id;

	private:
		void startStream(std::string_view userName, std::string_view roomName, std::string_view roomPassword, uint64_t id, uint32_t width, uint32_t height, bool showPreview, uint32_t frameRate);

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		StartStream(std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, client::Settings& settings, std::jthread& streamThread, uint64_t id, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~StartStream() = default;
	};
}
