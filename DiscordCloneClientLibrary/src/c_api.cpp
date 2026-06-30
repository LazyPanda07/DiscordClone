#include "c_api.h"

#include <chrono>

#include <UDPClientSocket.hpp>
#include <PatternParser.h>

#include "Functionality.hpp"
#include "Microphone.hpp"
#include "Speaker.hpp"

static constexpr uint32_t sampleRate = 48'000;
static constexpr uint32_t frameSize = 480;

template<>
struct utility::parsers::Converter<int32_t>
{
	constexpr void convert(std::string_view data, int32_t& result)
	{
		result = std::stoi(data.data());
	}
};

void initialize(Exception* exception)
{
	try
	{
		
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

UdpSocketObject createSocket(const char* ip, uint16_t port, Exception* exception)
{
	try
	{
		return new web::UDPClientSocket(ip, port);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

MicrophoneObject createMicrophone(UdpSocketObject socket, Exception* exception)
{
	try
	{
		return new voice::Microphone(*reinterpret_cast<web::UDPSocket*>(socket), frameSize, sampleRate);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

SpeakerObject createSpeaker(UdpSocketObject socket, Exception* exception)
{
	try
	{
		return new voice::Speaker(*reinterpret_cast<web::UDPSocket*>(socket), frameSize, sampleRate);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sendData(UdpSocketObject socket, const char* data, uint64_t size, Exception* exception)
{
	try
	{
		static_cast<web::UDPSocket*>(socket)->sendData(std::string_view(data, size));
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void receiveData(UdpSocketObject socket, void(*callback)(const char* data, uint64_t size, void* userData), int32_t flags, void* userData, Exception* exception)
{
	try
	{
		static_cast<web::UDPSocket*>(socket)->receiveData
		(
			[callback, userData](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
			{
				if (size == SOCKET_ERROR)
				{
#ifdef __LINUX__
					throw std::runtime_error(std::format("Can't send data: {}", strerror(errno)));
#else

					throw std::runtime_error(std::format("Can't send data: {}", WSAGetLastError()));
#endif
				}

				callback(data.data(), size, userData);
			},
			flags
		);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void overrideMicrophoneDeviceId(MicrophoneObject microphone, uint32_t id, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->overrideDeviceId(id);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void muteOrUnmute(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		functionality::muteOrUnmute(*static_cast<voice::Microphone*>(microphone));
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

bool isStreamRunning(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		return static_cast<voice::Microphone*>(microphone)->isStreamRunning();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

void restartMicrophone(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->restart();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void sendSilence(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->sendSilence();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void setMicrophoneVolume(MicrophoneObject microphone, double volume, Exception* exception)
{
	try
	{
		static_cast<voice::Microphone*>(microphone)->setVolume(volume);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

double getMicrophoneVolume(MicrophoneObject microphone, Exception* exception)
{
	try
	{
		return static_cast<voice::Microphone*>(microphone)->getVolume();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0.0;
}

void overrideSpeakerDeviceId(SpeakerObject speaker, uint32_t id, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(speaker)->overrideDeviceId(id);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void restartSpeaker(SpeakerObject speaker, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(speaker)->restart();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void setSpeakerVolume(SpeakerObject microphone, double volume, Exception* exception)
{
	try
	{
		static_cast<voice::Speaker*>(microphone)->setVolume(volume);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

double getSpeakerVolume(SpeakerObject microphone, Exception* exception)
{
	try
	{
		return static_cast<voice::Speaker*>(microphone)->getVolume();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0.0;
}

DeviceInformationArray getDeviceInformation(Exception* exception)
{
	try
	{
		std::vector<RtAudio::DeviceInfo>* result = new std::vector<RtAudio::DeviceInfo>();
		std::vector<RtAudio::DeviceInfo> devices = functionality::getAudioDevices();

		for (RtAudio::DeviceInfo& device : devices)
		{
			result->emplace_back(std::move(device));
		}

		return result;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

uint64_t getDeviceInformationSize(DeviceInformationArray deviceInformation, Exception* exception)
{
	try
	{
		return static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation)->size();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

uint32_t getDeviceInformationId(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].ID;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

const char* getDeviceInformationName(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].name.data();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

uint32_t getDeviceInformationInputChannels(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].inputChannels;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

uint32_t getDeviceInformationOutputChannels(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].outputChannels;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

bool getDeviceInformationDefaultInput(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].isDefaultInput;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

bool getDeviceInformationDefaultOutput(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception)
{
	try
	{
		return (*static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation))[index].isDefaultOutput;
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

const char* getVersion(Exception* exception)
{
	try
	{
		return functionality::getDiscordCloneClientLibraryVersion().data();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

void getVersionExtended(int32_t* major, int32_t* minor, int32_t* patch, Exception* exception)
{
	constexpr utility::parsers::PatternParser<int32_t, int32_t, int32_t> parser("{}.{}.{}");

	try
	{
		parser.parse(functionality::getDiscordCloneClientLibraryVersion(), *major, *minor, *patch);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}	
}

const char* getExceptionMessage(Exception exception)
{
	if (!exception)
	{
		return nullptr;
	}

	return static_cast<std::runtime_error*>(exception)->what();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void deleteSocket(UdpSocketObject socket)
{
	delete static_cast<web::UDPSocket*>(socket);
}

void deleteMicrophone(MicrophoneObject microphone)
{
	delete static_cast<voice::Microphone*>(microphone);
}

void deleteSpeaker(SpeakerObject speaker)
{
	delete static_cast<voice::Speaker*>(speaker);
}

void deleteException(Exception exception)
{
	delete static_cast<std::runtime_error*>(exception);
}

void deleteDeviceInformation(DeviceInformationArray deviceInformation)
{
	delete static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation);
}
