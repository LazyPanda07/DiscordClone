#pragma once

#include "Command.hpp"

#include <thread>

#include <IOSocketStream.h>

#include "Wrappers/SocketWrapper.hpp"
#include "Settings.hpp"

namespace commands
{
	class StopStream : public Command
	{
	private:
		std::unique_ptr<streams::IOSocketStream>& controlStream;
		std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket;
		std::jthread& streamThread;
		const client::Settings& settings;

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		StopStream(std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, std::jthread& streamThread, const client::Settings& settings, const std::vector<std::unique_ptr<checks::Check>>& checks);

		~StopStream() = default;
	};
}
