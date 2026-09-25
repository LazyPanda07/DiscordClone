#pragma once

#include "Command.hpp"

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
		const client::Settings& settings;
		uint64_t id;

	private:
		void startStream(std::string_view userName, std::string_view roomName, std::string_view roomPassword, uint64_t id);

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		StartStream(std::unique_ptr<streams::IOSocketStream>& controlStream, std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& videoStreamSocket, const client::Settings& settings, uint64_t id, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~StartStream() = default;
	};
}
