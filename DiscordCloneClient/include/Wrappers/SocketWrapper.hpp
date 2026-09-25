#pragma once

#include <string>

#include <c_api.h>

#include "Utils.hpp"

namespace wrappers
{
	enum class SocketType : uint8_t
	{
		udp,
		tcp
	};

	template<SocketType T>
	class SocketWrapper
	{
	private:
		template<SocketType ValueT>
		struct GetSocketType
		{
			static_assert(false, "Wrong type");
		};

		template<>
		struct GetSocketType<SocketType::udp>
		{
			using type = UdpSocketObject;
		};

		template<>
		struct GetSocketType<SocketType::tcp>
		{
			using type = TcpSocketObject;
		};

	private:
		GetSocketType<T>::type implementation;

	public:
		SocketWrapper(std::string_view ip, uint16_t port);

		void sendData(std::string_view data);

		std::string receiveData(int32_t flags = 0);

		~SocketWrapper();

		friend class MicrophoneWrapper;
		friend class SpeakerWrapper;
	};
}

namespace wrappers
{
	template<SocketType T>
	SocketWrapper<T>::SocketWrapper(std::string_view ip, uint16_t port)
	{
		if constexpr (T == SocketType::udp)
		{
			implementation = utils::callApiFunction(&::createSocket, ip.data(), port);
		}
		else
		{
			implementation = utils::callApiFunction(&::createTcpSocket, ip.data(), port);
		}
	}

	template<SocketType T>
	void SocketWrapper<T>::sendData(std::string_view data)
	{
		if constexpr (T == SocketType::udp)
		{
			utils::callApiFunction(&::sendData, implementation, data.data(), data.size());
		}
		else
		{
			utils::callApiFunction(&::sendTcpData, implementation, data.data(), data.size());
		}
	}

	template<SocketType T>
	std::string SocketWrapper<T>::receiveData(int32_t flags)
	{
		std::string result;

		auto callback = [](const char* data, uint64_t size, void* userData)
			{
				static_cast<std::string*>(userData)->append(data, size);
			};

		if constexpr (T == SocketType::udp)
		{
			utils::callApiFunction(&::receiveData, implementation, callback, flags, &result);
		}
		else
		{
			utils::callApiFunction(&::receiveNotification, implementation, callback, &result);
		}

		return result;
	}

	template<SocketType T>
	SocketWrapper<T>::~SocketWrapper()
	{
		if constexpr (T == SocketType::udp)
		{
			deleteSocket(implementation);
		}
		else
		{
			deleteTcpSocket(implementation);
		}
		
		implementation = nullptr;
	}
}
