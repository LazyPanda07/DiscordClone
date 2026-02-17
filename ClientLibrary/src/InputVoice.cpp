#include "InputVoice.hpp"

#include <span>
#include <UUID.h>

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#endif

#include "Generated/microphone_on.generated.hpp"
#include "Generated/microphone_off.generated.hpp"

namespace voice
{
	int InputVoice::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
		InputVoice& voice = *reinterpret_cast<InputVoice*>(userData);

		if (status & RTAUDIO_INPUT_OVERFLOW)
		{
			// TODO: handle
		}

		if (!inputBuffer)
		{
			return 0;
		}

		std::span<float> in(static_cast<float*>(inputBuffer), frames * voice.parameters.nChannels);

		if (voice.volume != 1.0)
		{
			for (float& value : in)
			{
				value *= voice.volume;
			}
		}

		voice.socket.sendData(in);

		return 0;
	}

	InputVoice::InputVoice(web::UDPSocket& socket, uint32_t bufferFrames, uint32_t sampleRate) :
		socket(socket),
		volume(1.0),
		bufferFrames(bufferFrames),
		sampleRate(sampleRate)
	{
		parameters.deviceId = audio.getDefaultInputDevice();
		parameters.nChannels = 1;

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &InputVoice::callback, this);

		audio.startStream();
	}

	void InputVoice::overrideDeviceId(uint32_t id)
	{
		parameters.deviceId = id;
	}

	void InputVoice::restart()
	{
		audio.stopStream();
		audio.closeStream();

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &InputVoice::callback, this);
	}

	void InputVoice::startStream()
	{
#ifdef _WIN32
		PlaySound(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(microphoneOnWav)), nullptr, SND_MEMORY | SND_SYNC);
#endif

		audio.startStream();
	}

	void InputVoice::stopStream()
	{
#ifdef _WIN32
		PlaySound(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(microphoneOffWav)), nullptr, SND_MEMORY | SND_SYNC);
#endif

		audio.stopStream();
	}

	bool InputVoice::isStreamRunning() const
	{
		return audio.isStreamRunning();
	}

	void InputVoice::setVolume(double volume)
	{
		this->volume = volume;
	}

	double InputVoice::getVolume() const
	{
		return volume;
	}

	InputVoice::~InputVoice()
	{

	}
}
