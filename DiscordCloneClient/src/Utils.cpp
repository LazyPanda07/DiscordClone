#include "Utils.hpp"

#include <chrono>

#include "Wrappers/SocketWrapper.hpp"

namespace utils
{
	void runStream(std::stop_token stop, uint32_t width, uint32_t height, bool showPreview, uint32_t frameRate, void* socket)
	{
		using namespace std::chrono_literals;

		std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& udpSocket = *reinterpret_cast<std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>*>(socket);
		ScreenCapturer capturer = callApiFunction(&::startStream, width, height, 0, showPreview);
		std::chrono::milliseconds desiredFrameTime(1s / frameRate);

		while (!stop.stop_requested())
		{
			auto start = std::chrono::steady_clock::now();

			callApiFunction(&::processFrame, **udpSocket, capturer);

			auto end = std::chrono::steady_clock::now();

			if (std::chrono::milliseconds frameTime(std::chrono::duration_cast<std::chrono::milliseconds>(end - start)); frameTime < desiredFrameTime)
			{
				std::this_thread::sleep_for(desiredFrameTime - frameTime);
			}
		}

		callApiFunction(&::stopStream, capturer);
	}

	void runViewStream(std::stop_token stop, int32_t width, int32_t height, void* socket)
	{
		using namespace std::chrono_literals;

		std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>& udpSocket = *reinterpret_cast<std::unique_ptr<wrappers::SocketWrapper<wrappers::SocketType::udp>>*>(socket);
		ScreenViewer viewer = callApiFunction(&::startStreamView, width, height);

		while (!stop.stop_requested())
		{
			callApiFunction(&::decodeFrame, **udpSocket, viewer);
		}

		callApiFunction(&::stopStreamView, viewer);
	}
}
