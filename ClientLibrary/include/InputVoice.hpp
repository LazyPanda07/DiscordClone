#pragma once

#include <RtAudio.h>

#include <UDPSocket.hpp>

namespace voice
{
	class InputVoice
	{
	private:
		static int callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData);

	private:
		std::string uuid;
		RtAudio audio;
		web::UDPSocket& socket;
		RtAudio::StreamParameters parameters;

	public:
		InputVoice(web::UDPSocket& socket, uint32_t bufferFrames, uint32_t sampleRate);

		~InputVoice();
	};
}
