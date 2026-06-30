#include "Microphone.hpp"

#include <span>
#include <filesystem>
#include <thread>

#ifdef __LINUX__
#include <dlfcn.h>
#else
#include <Windows.h>
#include <mmsystem.h>
#endif

#include "Functionality.hpp"
#include "Constants.hpp"

#ifdef __LINUX__
using HMODULE = void*;
#endif

using GetResourceSignature = const uint8_t* (*)(uint64_t*);

static void* resourceLibrary = nullptr;

static void loadResourceLibrary();

static const uint8_t* callGetMicrophoneOffSound(uint64_t* size);

static const uint8_t* callGetMicrophoneOnSound(uint64_t* size);

namespace voice
{
	int Microphone::callback(void* outputBuffer, void* inputBuffer, uint32_t frames, double streamTime, RtAudioStreamStatus status, void* userData)
	{
		Microphone& microphone = *reinterpret_cast<Microphone*>(userData);

		if (status & RTAUDIO_INPUT_OVERFLOW)
		{
			// TODO: handle
		}

		if (!inputBuffer)
		{
			return 0;
		}

		std::span<float> in(static_cast<float*>(inputBuffer), frames * microphone.parameters.nChannels);

		if (microphone.volume != 1.0)
		{
			for (float& value : in)
			{
				value *= microphone.volume;
			}
		}

		int bytes = opus_encode_float(microphone.encoder, in.data(), frames, microphone.outputData.data(), microphone.outputData.size());

		if (bytes < 0)
		{
			throw std::runtime_error(std::format("Fail encoding: {}", opus_strerror(bytes)));
		}

		microphone.socket.sendData(std::span<uint8_t>(microphone.outputData.data(), bytes));

		return 0;
	}

	Microphone::Microphone(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate) :
		socket(socket),
		volume(1.0),
		frameSize(frameSize),
		sampleRate(sampleRate),
		encoder(nullptr),
		outputData({})
	{
		loadResourceLibrary();

		audio.showWarnings();

		parameters.deviceId = audio.getDefaultInputDevice();
		parameters.nChannels = 1;

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &frameSize, &Microphone::callback, this);

		audio.startStream();

		int errorCode = 0;

		if (encoder = opus_encoder_create(sampleRate, 1, OPUS_APPLICATION_VOIP, &errorCode); errorCode != OPUS_OK)
		{
			throw std::runtime_error(std::format("Can't create Opus encoder: {}", opus_strerror(errorCode)));
		}

		std::thread
		(
			[this](std::function<bool& ()>& runningGetter)
			{
				// TODO: maybe change to std::shared_ptr

				using namespace std::chrono_literals;

				bool running = true;

				runningGetter = [&running]() -> bool&
					{
						return running;
					};

				while (running)
				{
					if (!audio.isStreamRunning())
					{
						this->sendSilence();
					}

					std::this_thread::sleep_for(1s);
				}
			},
			std::ref(runningGetter)
		).detach();
	}

	void Microphone::overrideDeviceId(uint32_t id)
	{
		parameters.deviceId = id;
	}

	void Microphone::restart()
	{
		if (audio.isStreamRunning())
		{
			audio.abortStream();
		}

		if (audio.isStreamOpen())
		{
			audio.closeStream();
		}

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &frameSize, &Microphone::callback, this);
		audio.startStream();
	}

	void Microphone::startStream()
	{
		uint64_t size = 0;

#ifndef __LINUX__
		PlaySoundA(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(callGetMicrophoneOnSound(&size))), nullptr, SND_MEMORY | SND_ASYNC);
#endif

		audio.startStream();
	}

	void Microphone::stopStream()
	{
		uint64_t size = 0;

#ifndef __LINUX__
		PlaySoundA(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(callGetMicrophoneOffSound(&size))), nullptr, SND_MEMORY | SND_ASYNC);
#endif

		audio.stopStream();
	}

	bool Microphone::isStreamRunning() const
	{
		return audio.isStreamRunning();
	}

	void Microphone::sendSilence()
	{
		int bytes = opus_encode_float(encoder, constants::silence.data(), constants::silence.size(), outputData.data(), outputData.size());

		if (bytes < 0)
		{
			throw std::runtime_error(std::format("Fail encoding: {}", opus_strerror(bytes)));
		}

		socket.sendData(std::span<uint8_t>(outputData.data(), bytes));
	}

	void Microphone::setVolume(double volume)
	{
		this->volume = volume;
	}

	double Microphone::getVolume() const
	{
		return volume;
	}

	Microphone::~Microphone()
	{
		if (encoder)
		{
			opus_encoder_destroy(encoder);

			encoder = nullptr;
		}

		runningGetter() = false;
	}
}

void loadResourceLibrary()
{
	if (resourceLibrary)
	{
		return;
	}

	std::filesystem::path currentPath(std::filesystem::current_path());

#ifdef __LINUX__
	currentPath /= "libDiscordCloneClientResources.so";

	resourceLibrary = dlopen(currentPath.string().data(), RTLD_LAZY);
#else
	currentPath /= "DiscordCloneClientResources.dll";

	resourceLibrary = LoadLibraryA(currentPath.string().data());
#endif
}

const uint8_t* callGetMicrophoneOffSound(uint64_t* size)
{
	if (!resourceLibrary)
	{
		return nullptr;
	}

#ifdef __LINUX__
	return reinterpret_cast<GetResourceSignature>(dlsym(resourceLibrary, "getMicrophoneOffSound"))(size);
#else
	return reinterpret_cast<GetResourceSignature>(GetProcAddress(static_cast<HMODULE>(resourceLibrary), "getMicrophoneOffSound"))(size);
#endif
}

const uint8_t* callGetMicrophoneOnSound(uint64_t* size)
{
	if (!resourceLibrary)
	{
		return nullptr;
	}

#ifdef __LINUX__
	return reinterpret_cast<GetResourceSignature>(dlsym(resourceLibrary, "getMicrophoneOnSound"))(size);
#else
	return reinterpret_cast<GetResourceSignature>(GetProcAddress(static_cast<HMODULE>(resourceLibrary), "getMicrophoneOnSound"))(size);
#endif
}
