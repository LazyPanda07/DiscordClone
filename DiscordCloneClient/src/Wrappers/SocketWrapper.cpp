#include "Wrappers/SocketWrapper.hpp"

#include "Utils.hpp"

static void callback(const char* data, uint64_t size, void* userData);

namespace wrapper
{
	SocketWrapper::SocketWrapper(std::string_view ip, uint16_t port) :
		implementation(utils::callApiFunction(&createSocket, ip.data(), port))
	{

	}

	void SocketWrapper::sendData(std::string_view data)
	{
		utils::callApiFunction(&::sendData, implementation, data.data(), data.size());
	}

	std::string SocketWrapper::receiveData()
	{
		std::string result;

		utils::callApiFunction(&::receiveData, implementation, &callback, &result);

		return result;
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
