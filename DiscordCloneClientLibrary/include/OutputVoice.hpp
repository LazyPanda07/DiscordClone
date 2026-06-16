#pragma once

#include <mutex>
#include <vector>
#include <span>

#include <RtAudio.h>
#include <opus.h>
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

#include <UDPSocket.hpp>

namespace voice
{
	class OutputVoice
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
		SpeexEchoState* echoState;
		SpeexPreprocessState* preprocessState;
		
	public:
		OutputVoice(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate, SpeexEchoState* echoState, SpeexPreprocessState* preprocessState);

		void overrideDeviceId(uint32_t id);

		void restart();

		void setVolume(double volume);

		double getVolume() const;

		~OutputVoice() = default;
	};
}
