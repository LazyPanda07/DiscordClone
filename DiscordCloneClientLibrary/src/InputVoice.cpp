#include "InputVoice.hpp"

#include <span>
#include <filesystem>

#include <UUID.h>

#ifdef __LINUX__
#include <dlfcn.h>
#else
#include <Windows.h>
#include <mmsystem.h>
#endif

#ifdef __LINUX__
using HMODULE = void*;
#endif

using GetResourceSignature = const uint8_t* (*)(uint64_t*);

static void* resourceLibrary = nullptr;

static void loadResourceLibrary();

static const uint8_t* callGetMicrophoneOffSound(uint64_t* size);

static const uint8_t* callGetMicrophoneOnSound(uint64_t* size);

#ifdef __LINUX__

#else

#endif

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
		loadResourceLibrary();

		audio.showWarnings();

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
		if (audio.isStreamRunning())
		{
			audio.abortStream();
		}

		if (audio.isStreamOpen())
		{
			audio.closeStream();
		}

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &InputVoice::callback, this);
		audio.startStream();
	}

	void InputVoice::startStream()
	{
		uint64_t size = 0;

#ifndef __LINUX__
		PlaySound(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(callGetMicrophoneOnSound(&size))), nullptr, SND_MEMORY | SND_ASYNC);
#endif

		audio.startStream();
	}

	void InputVoice::stopStream()
	{
		uint64_t size = 0;

#ifndef __LINUX__
		PlaySound(reinterpret_cast<PTCHAR>(const_cast<uint8_t*>(callGetMicrophoneOffSound(&size))), nullptr, SND_MEMORY | SND_ASYNC);
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
