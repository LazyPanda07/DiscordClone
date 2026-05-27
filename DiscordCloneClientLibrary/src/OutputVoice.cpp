#include "OutputVoice.hpp"

namespace voice
{
	int OutputVoice::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
		OutputVoice& voice = *reinterpret_cast<OutputVoice*>(userData);

		if (status & RTAUDIO_OUTPUT_UNDERFLOW)
		{
			// TODO: handle
		}

		if (!outputBuffer)
		{
			return 0;
		}

		try
		{
			voice.socket.receiveData
			(
				[outputBuffer, frames, &voice](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
				{
					if (size != web::UDPSocket::voicePacketSize)
					{
						return;
					}

					std::span<float> out(static_cast<float*>(outputBuffer), frames * voice.parameters.nChannels);
					std::span<const float> floatData(reinterpret_cast<const float*>(data.data()), size / sizeof(float));

					for (size_t i = 0; i < frames; i++)
					{
						out[i * 2] = floatData[i] * voice.volume;
						out[i * 2 + 1] = floatData[i] * voice.volume;
					}
				}
			);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;

			exit(0);
		}

		return 0;
	}

	OutputVoice::OutputVoice(web::UDPSocket& socket, uint32_t bufferFrames, uint32_t sampleRate) :
		socket(socket),
		volume(1.0),
		bufferFrames(bufferFrames),
		sampleRate(sampleRate)
	{
		audio.showWarnings();

		parameters.deviceId = audio.getDefaultOutputDevice();
		parameters.nChannels = 2;

		audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &OutputVoice::callback, this);

		audio.startStream();
	}

	void OutputVoice::overrideDeviceId(uint32_t id)
	{
		parameters.deviceId = id;
	}

	void OutputVoice::restart()
	{
		if (audio.isStreamRunning())
		{
			audio.abortStream();
		}

		if (audio.isStreamOpen())
		{
			audio.closeStream();
		}

		audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &OutputVoice::callback, this);
		audio.startStream();
	}

	void OutputVoice::setVolume(double volume)
	{
		this->volume = volume;
	}

	double OutputVoice::getVolume() const
	{
		return volume;
	}
}
