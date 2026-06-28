#pragma once

#include "Command.hpp"

#include <functional>

#include <IOSocketStream.h>

#include "Wrappers/SocketWrapper.hpp"
#include "Settings.hpp"

namespace commands
{
	class Connect : public Command
	{
	private:
		std::unique_ptr<wrappers::SocketWrapper>& socket;
		std::unique_ptr<streams::IOSocketStream>& controlStream;
		client::Settings& settings;
		std::function<void(uint64_t&)> onSuccess;

	private:
		bool connect(std::string_view ip, uint16_t port, std::string_view userName, std::string_view roomName, std::string_view roomPassword, uint64_t& id);

		bool sendHello(uint64_t id);

		bool receiveHello(uint64_t id);

	private:
		bool run(std::istream& stream) override;

		uint32_t getChecks() const override;

	public:
		Connect(std::unique_ptr<wrappers::SocketWrapper>& socket, std::unique_ptr<streams::IOSocketStream>& controlStream, client::Settings& settings, const std::function<void(uint64_t&)>& onSuccess, const std::vector<std::unique_ptr<checks::Check>>& checks);

		std::string_view getHelpText() const override;

		~Connect() = default;
	};
}
