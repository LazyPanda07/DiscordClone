#include "Wrappers/SocketWrapper.hpp"

#include "Utils.hpp"

static void callback(const char* data, uint64_t size, void* userData);

namespace wrappers
{
	SocketWrapper::SocketWrapper(std::string_view ip, uint16_t port) :
		implementation(utils::callApiFunction(&createSocket, ip.data(), port))
	{

	}

	void SocketWrapper::sendData(std::string_view data)
	{
		utils::callApiFunction(&::sendData, implementation, data.data(), data.size());
	}

	std::string SocketWrapper::receiveData(int32_t flags)
	{
		std::string result;

		utils::callApiFunction(&::receiveData, implementation, &callback, flags, &result);

		return result;
	}

	int64_t SocketWrapper::ping() const
	{
		return utils::callApiFunction(&::ping, implementation);
	}

	SocketWrapper::~SocketWrapper()
	{
		deleteSocket(implementation);

		implementation = nullptr;
	}
}

void callback(const char* data, uint64_t size, void* userData)
{
	static_cast<std::string*>(userData)->append(data, size);
}
