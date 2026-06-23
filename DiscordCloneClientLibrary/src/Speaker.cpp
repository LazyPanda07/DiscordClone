#include "Speaker.hpp"

template<size_t Size>
static void fillSound(const std::array<float, Size>& sound, std::span<float> out);

namespace voice
{
	int Speaker::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
		static constexpr std::array<float, web::UDPSocket::voicePacketSize / sizeof(float)> silence{};

		Speaker& voice = *reinterpret_cast<Speaker*>(userData);
		std::span<float> out(static_cast<float*>(outputBuffer), frames * voice.parameters.nChannels);

		if (status & RTAUDIO_OUTPUT_UNDERFLOW)
		{
			// TODO: handle
		}

		if (!outputBuffer)
		{
			fillSound(silence, out);

			return 0;
		}

		try
		{
			bool result = voice.socket.receiveData
			(
				[outputBuffer, frames, &voice, out](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
				{
					if (size == SOCKET_ERROR)
					{
						fillSound(silence, out);

						return;
					}

					int samples = opus_decode_float(voice.decoder, reinterpret_cast<const uint8_t*>(data.data()), size, voice.inputDataBuffer.data(), voice.frameSize, 0);

					if (voice.volume != 1.0)
					{
						for (float& value : voice.inputDataBuffer)
						{
							value *= voice.volume;
						}
					}

					fillSound(voice.inputDataBuffer, out);
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

	Speaker::Speaker(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate) :
		socket(socket),
		volume(1.0),
		frameSize(frameSize),
		sampleRate(sampleRate),
		decoder(nullptr),
		inputDataBuffer({})
	{
		audio.showWarnings();

		parameters.deviceId = audio.getDefaultOutputDevice();
		parameters.nChannels = 2;

		audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &frameSize, &Speaker::callback, this);
		audio.startStream();

		int errorCode = 0;

		if (decoder = opus_decoder_create(sampleRate, 1, &errorCode); errorCode != OPUS_OK)
		{
			throw std::runtime_error(std::format("Can't create Opus decoder: {}", opus_strerror(errorCode)));
		}
	}

	void Speaker::overrideDeviceId(uint32_t id)
	{
		parameters.deviceId = id;
	}

	void Speaker::restart()
	{
		if (audio.isStreamRunning())
		{
			audio.abortStream();
		}

		if (audio.isStreamOpen())
		{
			audio.closeStream();
		}

		audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &frameSize, &Speaker::callback, this);
		audio.startStream();
	}

	void Speaker::setVolume(double volume)
	{
		this->volume = volume;
	}

	double Speaker::getVolume() const
	{
		return volume;
	}
}

template<size_t Size>
void fillSound(const std::array<float, Size>& sound, std::span<float> out)
{
	if (sound.size() * 2 != out.size())
	{
		std::cerr << std::format("Wrong fill size. Sound: {}, out: {}", sound.size(), out.size()) << std::endl;

		return;
	}

	for (size_t i = 0; i < sound.size(); i++)
	{
		out[i * 2] = sound[i];
		out[i * 2 + 1] = sound[i];
	}
}
