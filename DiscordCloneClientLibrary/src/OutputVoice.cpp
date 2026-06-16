#include "OutputVoice.hpp"

#include "Functionality.hpp"

namespace voice
{
	int OutputVoice::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
		constexpr std::array<int16_t, web::UDPSocket::voicePacketSize / sizeof(float)> silence{};

		OutputVoice& voice = *reinterpret_cast<OutputVoice*>(userData);

		if (status & RTAUDIO_OUTPUT_UNDERFLOW)
		{
			// TODO: handle
		}

		if (!outputBuffer)
		{
			speex_echo_playback(voice.echoState, silence.data());

			return 0;
		}

		try
		{
			bool result = voice.socket.receiveData
			(
				[outputBuffer, frames, &voice](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
				{
					if (size == SOCKET_ERROR)
					{
						return;
					}

					std::span<float> out(static_cast<float*>(outputBuffer), frames * voice.parameters.nChannels);
					std::array<float, web::UDPSocket::voicePacketSize / sizeof(float)> input{};
					std::array<int16_t, web::UDPSocket::voicePacketSize / sizeof(float)> echoCancelation{};
					int samples = opus_decode_float(voice.decoder, reinterpret_cast<const uint8_t*>(data.data()), size, input.data(), voice.frameSize, 0);

					functionality::toInt(input, echoCancelation);

					speex_echo_playback(voice.echoState, echoCancelation.data());

					if (voice.volume != 1.0)
					{
						for (float& value : input)
						{
							value *= voice.volume;
						}
					}

					for (size_t i = 0; i < samples; i++)
					{
						out[i * 2] = input[i];
						out[i * 2 + 1] = input[i];
					}
				},
				web::UDPSocket::customNonBlockingFlag
			);

			if (!result)
			{
				speex_echo_playback(voice.echoState, silence.data());
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;

			exit(0);
		}

		return 0;
	}

	OutputVoice::OutputVoice(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate, SpeexEchoState* echoState, SpeexPreprocessState* preprocessState) :
		socket(socket),
		volume(1.0),
		frameSize(frameSize),
		sampleRate(sampleRate),
		decoder(nullptr),
		echoState(echoState),
		preprocessState(preprocessState)
	{
		audio.showWarnings();

		parameters.deviceId = audio.getDefaultOutputDevice();
		parameters.nChannels = 2;

		audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &frameSize, &OutputVoice::callback, this);

		audio.startStream();

		int errorCode = 0;

		if (decoder = opus_decoder_create(sampleRate, 1, &errorCode); errorCode != OPUS_OK)
		{
			throw std::runtime_error(std::format("Can't create Opus decoder: {}", opus_strerror(errorCode)));
		}
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

		audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &frameSize, &OutputVoice::callback, this);
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
