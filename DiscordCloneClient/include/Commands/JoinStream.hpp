#pragma once

#include "Command.hpp"

#include <thread>

#include <IOSocketStream.h>

#include "Wrappers/SocketWrapper.hpp"
#include "Settings.hpp"

namespace commands
{
	class JoinStream : public Command
	{
	private:
		const std::unique_ptr<streams::IOSocketStream>& controlStream;
		std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket;
		const client::Settings& settings;
		std::jthread& streamThread;
		uint64_t id;

	private:
		bool receiveHello(uint64_t id);

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		JoinStream(const std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, std::jthread& streamThread, const client::Settings& settings, uint64_t id, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~JoinStream() = default;
	};
}
