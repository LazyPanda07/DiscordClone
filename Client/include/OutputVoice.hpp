#pragma once

#include <mutex>
#include <vector>
#include <span>

#include <RtAudio.h>

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
		
	public:
		OutputVoice(web::UDPSocket& socket, uint32_t bufferFrames, uint32_t sampleRate);

		~OutputVoice();
	};
}
