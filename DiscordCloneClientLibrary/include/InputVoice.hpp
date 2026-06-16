#pragma once

#include <RtAudio.h>
#include <opus.h>
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

#include <UDPSocket.hpp>

namespace voice
{
	class InputVoice
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
		SpeexEchoState* echoState;
		SpeexPreprocessState* preprocessState;
		std::array<int16_t, web::UDPSocket::voicePacketSize / sizeof(float)> intInData;
		std::array<int16_t, web::UDPSocket::voicePacketSize / sizeof(float)> intOutData;
		std::array<float, web::UDPSocket::voicePacketSize / sizeof(float)> floatOutData;
		std::array<uint8_t, web::UDPSocket::voicePacketSize> outputData;

	public:
		InputVoice(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate, SpeexEchoState* echoState, SpeexPreprocessState* preprocessState);

		void overrideDeviceId(uint32_t id);

		void restart();

		void startStream();

		void stopStream();

		bool isStreamRunning() const;

		void setVolume(double volume);

		double getVolume() const;

		~InputVoice();
	};
}
