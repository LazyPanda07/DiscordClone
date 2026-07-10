#pragma once

#include <mutex>
#include <vector>
#include <span>
#include <optional>

#include <RtAudio.h>
#include <opus.h>

#include <UDPSocket.hpp>

namespace voice
{
	class Speaker
	{
	private:
		static int callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData);

		void receiveSound(std::function<std::optional<web::UDPSocket::VoicePacket>()>& soundGetter, std::function<bool&()>& runningGetter);

	private:
		RtAudio audio;
		web::UDPSocket& socket;
		RtAudio::StreamParameters parameters;
		double volume;
		uint32_t frameSize;
		uint32_t sampleRate;
		OpusDecoder* decoder;
		web::UDPSocket::VoicePacket inputDataBuffer;
		bool fixDelay;
		std::function<std::optional<web::UDPSocket::VoicePacket>()> soundGetter;
		std::function<bool& ()> runningGetter;

	public:
		Speaker(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate);

		void overrideDeviceId(uint32_t id);

		void restart();

		void fixSpeakerDelay();

		void setVolume(double volume);

		double getVolume() const;

		~Speaker();
	};
}
