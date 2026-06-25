#pragma once

#include <RtAudio.h>
#include <opus.h>

#include <UDPSocket.hpp>

namespace voice
{
	class Microphone
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
		OpusEncoder* encoder;
		std::array<uint8_t, web::UDPSocket::voicePacketSize> outputData;
		std::function<bool& ()> runningGetter;

	public:
		Microphone(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate);

		void overrideDeviceId(uint32_t id);

		void restart();

		void startStream();

		void stopStream();

		bool isStreamRunning() const;

		void setVolume(double volume);

		double getVolume() const;

		~Microphone();
	};
}
