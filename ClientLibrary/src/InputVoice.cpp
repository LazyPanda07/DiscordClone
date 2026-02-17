#include "InputVoice.hpp"

#include <span>
#include <UUID.h>

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#endif

namespace voice
{
	int InputVoice::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
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
		
		voice.socket.sendData(in);

		return 0;
	}

	InputVoice::InputVoice(web::UDPSocket& socket, uint32_t bufferFrames, uint32_t sampleRate) :
		uuid(utility::uuid::generateUUID()),
		socket(socket)
	{
		parameters.deviceId = audio.getDefaultInputDevice();
		parameters.nChannels = 1;

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &InputVoice::callback, this);

		this->startStream();
	}

	void InputVoice::startStream()
	{
#ifdef _WIN32
		
#endif

		audio.startStream();
	}

	void InputVoice::stopStream()
	{
#ifdef _WIN32

#endif

		audio.stopStream();
	}

	bool InputVoice::isStreamRunning() const
	{
		return audio.isStreamRunning();
	}

	InputVoice::~InputVoice()
	{

	}
}
