#include "Utils.hpp"

#include <chrono>

#include "Wrappers/SocketWrapper.hpp"

namespace utils
{
	void runStream(std::stop_token stop, uint32_t width, uint32_t height, bool showPreview, uint32_t frameRate, void* socket)
	{
		using namespace std::chrono_literals;

		std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& udpSocket = *reinterpret_cast<std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>*>(socket);
		ScreenCapturer capturer = callApiFunction(&::startStream, width, height, 0, true);
		std::chrono::milliseconds desiredFrameTime(1s);

		desiredFrameTime /= frameRate;

		while (!stop.stop_requested())
		{
			auto start = std::chrono::steady_clock::now();

			callApiFunction(&::processFrame, **udpSocket, capturer);

			auto end = std::chrono::steady_clock::now();

			std::chrono::milliseconds frameTime(std::chrono::duration_cast<std::chrono::milliseconds>(end - start));

			if (frameTime < desiredFrameTime)
			{
				std::this_thread::sleep_for(desiredFrameTime - frameTime);
			}
		}

		callApiFunction(&::stopStream, capturer);
	}
}
