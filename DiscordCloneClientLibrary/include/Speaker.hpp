#pragma once

#include <mutex>
#include <vector>
#include <span>

#include <RtAudio.h>
#include <opus.h>

#include <UDPSocket.hpp>

namespace voice
{
	class Speaker
	{
	private:
		static int callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData);

	private:
		RtAudio audio;
		web::UDPSocket& socket;
		RtAudio::StreamParameters parameters;
		double volume;
		uint32_t frameSize;
		uint32_t sampleRate;
		OpusDecoder* decoder;
		std::array<float, web::UDPSocket::voicePacketSize / sizeof(float)> inputDataBuffer;
		
	public:
		Speaker(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate);

		void overrideDeviceId(uint32_t id);

		void restart();

		void setVolume(double volume);

		double getVolume() const;

		~Speaker() = default;
	};
}
