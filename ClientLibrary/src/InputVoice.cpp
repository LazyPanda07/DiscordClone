#include "InputVoice.hpp"

#include <span>
#include <UUID.h>

namespace voice
{
	int InputVoice::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
		CLIENT_LIBRARY_API;

		InputVoice& voice = *reinterpret_cast<InputVoice*>(userData);

		if (status & RTAUDIO_INPUT_OVERFLOW)
		{
			std::cerr << "Audio input overflow detected!" << std::endl;
		}

		if (!inputBuffer)
		{
			return 0;
		}

		std::span<float> in(static_cast<float*>(inputBuffer), frames * voice.parameters.nChannels);
		web::UDPSocket::Buffer buffer{};
		size_t currentSize = 0;

		std::memcpy(buffer.data(), voice.uuid.data(), voice.uuid.size());
		currentSize += voice.uuid.size();

		std::memcpy(buffer.data() + currentSize, in.data(), in.size_bytes());
		currentSize += in.size_bytes();

		voice.socket.sendData(std::span<const char>(buffer.data(), currentSize));

		return 0;
	}

	InputVoice::InputVoice(web::UDPSocket& socket, uint32_t bufferFrames, uint32_t sampleRate) :
		uuid(utility::uuid::generateUUID()),
		socket(socket)
	{
		parameters.deviceId = audio.getDefaultInputDevice();
		parameters.nChannels = 1;

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &InputVoice::callback, this);

		audio.startStream();
	}

	InputVoice::~InputVoice()
	{

	}
}
