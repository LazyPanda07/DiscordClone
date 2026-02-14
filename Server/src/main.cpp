#include <iostream>
#include <thread>
#include <chrono>

#include <UDPSocket.hpp>

int main(int argc, char** argv)
{
	web::UDPSocket socket(8080);

	while (true)
	{
		std::string data = socket.receiveData();

		if (data.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));

			continue;
		}

		std::cout << data << std::endl;
	}


	return 0;
}
