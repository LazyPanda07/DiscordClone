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

#define closesocket ::close
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
	class TCPSocket
	{
	public:
		static constexpr size_t notificationSize = 64;

	public:
		static std::string constructHelloPacket(uint64_t id);

		static std::string constructJoinPacket(std::string_view userName);

	protected:
		SOCKET tcpSocket;

	protected:
		TCPSocket();

	public:
		template<typename DataT>
		int sendBytes(const DataT* const data, int size) const;

		template<typename DataT>
		int receiveBytes(DataT* const data, int size);

		void close();

		SOCKET getSocket() const;

		virtual ~TCPSocket();
	};
}

namespace web
{
	template<typename DataT>
	int TCPSocket::sendBytes(const DataT* const data, int size) const
	{
		int lastSend = 0;
		int totalSent = 0;

		do
		{
			lastSend = send(tcpSocket, reinterpret_cast<const char*>(data) + totalSent, size - totalSent, NULL);

			if (lastSend == SOCKET_ERROR)
			{
				throw std::runtime_error("Socket send error");
			}
			else if (!lastSend)
			{
				return totalSent;
			}

			totalSent += lastSend;
		}
		while (totalSent < size);

		return totalSent;
	}

	template<typename DataT>
	int TCPSocket::receiveBytes(DataT* const data, int size)
	{
		int totalReceive = 0;

		do
		{
			int lastReceive = recv(tcpSocket, reinterpret_cast<char*>(data) + totalReceive, size - totalReceive, NULL);

			if (lastReceive == SOCKET_ERROR)
			{
				throw std::runtime_error("Socket receive error");
			}
			else if (!lastReceive)
			{
				return totalReceive;
			}

			totalReceive += lastReceive;
		}
		while (totalReceive < size);

		return totalReceive;
	}
}
