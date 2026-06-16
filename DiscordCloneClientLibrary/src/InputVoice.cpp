#include "InputVoice.hpp"

#include <span>
#include <filesystem>

#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

#ifdef __LINUX__
#include <dlfcn.h>
#else
#include <Windows.h>
#include <mmsystem.h>
#endif

#include "Functionality.hpp"

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

		functionality::toInt(in, voice.intInData);

		speex_echo_capture(voice.echoState, voice.intInData.data(), voice.intOutData.data());

		functionality::toFloat(voice.intOutData, voice.floatOutData);

		int bytes = opus_encode_float(voice.encoder, voice.floatOutData.data(), frames, voice.outputData.data(), voice.outputData.size());

		if (bytes < 0)
		{
			throw std::runtime_error(std::format("Fail encoding: {}", opus_strerror(bytes)));
		}

		voice.socket.sendData(std::span<uint8_t>(voice.outputData.data(), bytes));

		return 0;
	}

	InputVoice::InputVoice(web::UDPSocket& socket, uint32_t frameSize, uint32_t sampleRate, SpeexEchoState* echoState, SpeexPreprocessState* preprocessState) :
		socket(socket),
		volume(1.0),
		frameSize(frameSize),
		sampleRate(sampleRate),
		encoder(nullptr),
		echoState(echoState),
		preprocessState(preprocessState),
		intInData({}),
		intOutData({}),
		floatOutData({}),
		outputData({})
	{
		loadResourceLibrary();

		audio.showWarnings();

		parameters.deviceId = audio.getDefaultInputDevice();
		parameters.nChannels = 1;

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &frameSize, &InputVoice::callback, this);

		audio.startStream();

		int errorCode = 0;

		if (encoder = opus_encoder_create(sampleRate, 1, OPUS_APPLICATION_VOIP, &errorCode); errorCode != OPUS_OK)
		{
			throw std::runtime_error(std::format("Can't create Opus encoder: {}", opus_strerror(errorCode)));
		}
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

		audio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &frameSize, &InputVoice::callback, this);
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

	InputVoice::~InputVoice()
	{
		if (encoder)
		{
			opus_encoder_destroy(encoder);

			encoder = nullptr;
		}
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
