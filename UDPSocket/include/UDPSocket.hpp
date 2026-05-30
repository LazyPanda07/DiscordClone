#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <format>
#include <array>
#include <span>
#include <stdexcept>
#include <format>
#include <cstring>
#include <optional>
#include <variant>

#ifdef __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

#ifndef WINDOWS_STYLE_DEFINITION
#define WINDOWS_STYLE_DEFINITION

#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

using SOCKET = int;
using DWORD = uint32_t;

#endif // WINDOWS_STYLE_DEFINITION
#else
#include <WinSock2.h>
#include <WS2tcpip.h>

using socklen_t = int;
#endif

namespace web
{
	class UDPSocket
	{
	public:
		using Buffer = std::array<char, 4096>;
		using ReceiveCallback = std::function<void(const Buffer& data, socklen_t size, const sockaddr_in& address, const UDPSocket& socket)>;

	public:
		static constexpr size_t voicePacketSize = 480 * sizeof(float);

		static constexpr std::string_view hello = "hello";
		static constexpr size_t helloPacketSize = hello.size();

		static constexpr std::string_view echo = "echo";
		static constexpr size_t echoPacketSize = echo.size();

	protected:
		SOCKET udpSocket;
		sockaddr_in address;

	protected:
		void initializeSockets();

	protected:
		UDPSocket(const sockaddr_in& address = {});

	public:
		template<typename T>
		int sendData(std::span<T> data, const std::optional<std::variant<const UDPSocket*, sockaddr_in>>& additionalData = nullptr) const;

		int sendData(std::string_view data, const UDPSocket* receiver = nullptr) const;

		int sendData(std::string_view data, const sockaddr_in& address) const;

		virtual void receiveData(const ReceiveCallback& callback) = 0;

		void close();

		virtual ~UDPSocket();
	};
}

namespace web
{
	template<typename T>
	int UDPSocket::sendData(std::span<T> data, const std::optional<std::variant<const UDPSocket*, sockaddr_in>>& additionalData) const
	{
		const sockaddr* toAddress = nullptr;

		if (additionalData)
		{
			if (std::holds_alternative<const UDPSocket*>(*additionalData))
			{
				const UDPSocket* temp = std::get<const UDPSocket*>(*additionalData);

				toAddress = temp ? reinterpret_cast<const sockaddr*>(&temp->address) : reinterpret_cast<const sockaddr*>(&address);
			}
			else if (std::holds_alternative<sockaddr_in>(*additionalData))
			{
				const sockaddr_in& temp = std::get<sockaddr_in>(*additionalData);

				toAddress = reinterpret_cast<const sockaddr*>(&temp);
			}
			else
			{
				throw std::runtime_error(std::format("Wrong additionalData type: ", typeid(*additionalData).name()));
			}
		}

		if (!toAddress)
		{
			throw std::runtime_error("Wrong toAddress initialization");
		}

		int totalSend = 0;

		while (totalSend != data.size_bytes())
		{
			int temp = sendto(udpSocket, reinterpret_cast<const char*>(data.data()) + totalSend, data.size_bytes() - totalSend, 0, toAddress, sizeof(sockaddr_in));

			if (temp == SOCKET_ERROR)
			{
#ifdef __LINUX__
				throw std::runtime_error(std::format("Can't send data: {}", strerror(errno)));
#else

				throw std::runtime_error(std::format("Can't send data: {}", WSAGetLastError()));
#endif
			}

			totalSend += temp;
		}
		
		return totalSend;
	}
}
