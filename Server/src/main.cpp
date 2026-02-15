#include <iostream>
#include <thread>
#include <chrono>

#include <UDPServerSocket.hpp>

int main(int argc, char** argv)
{
	std::unique_ptr<web::UDPSocket> socket = std::make_unique<web::UDPServerSocket>(8080);

	while (true)
	{
		socket->receiveData
		(
			[](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address)
			{
				std::cout << data.data() << std::endl;
			}
		);
	}

	return 0;
}
