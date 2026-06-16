#include "c_api.h"

#include <chrono>

#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

#include <UDPClientSocket.hpp>

#include "Functionality.hpp"
#include "InputVoice.hpp"
#include "OutputVoice.hpp"

static constexpr uint32_t sampleRate = 48'000;
static constexpr uint32_t frameSize = 480;
static constexpr int32_t tailLength = frameSize * 12;
static SpeexEchoState* echoState = nullptr;
static SpeexPreprocessState* preprocessState = nullptr;

void initializeEchoCancelation(Exception* exception)
{
	int32_t localSampleRate = sampleRate;
	int32_t denoise = 1;
	int32_t agc = 0;

	try
	{
		if (echoState = speex_echo_state_init(frameSize, tailLength); !echoState)
		{
			throw std::runtime_error("Can't initialize echo state");
		}

		if (speex_echo_ctl(echoState, SPEEX_ECHO_SET_SAMPLING_RATE, &localSampleRate))
		{
			throw std::runtime_error("Can't set sampling rate");
		}

		if (preprocessState = speex_preprocess_state_init(frameSize, localSampleRate); !preprocessState)
		{
			throw std::runtime_error("Can't initialize preprocess");
		}

		if (speex_preprocess_ctl(preprocessState, SPEEX_PREPROCESS_SET_ECHO_STATE, echoState))
		{
			throw std::runtime_error("Can't set echo state into preprocess");
		}

		if (speex_preprocess_ctl(preprocessState, SPEEX_PREPROCESS_SET_DENOISE, &denoise))
		{
			throw std::runtime_error("Can't set denoise to preprocess");
		}

		if (speex_preprocess_ctl(preprocessState, SPEEX_PREPROCESS_SET_AGC, &agc))
		{
			throw std::runtime_error("Can't unset Automatic Gain Control");
		}
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

InputVoiceStreamObject createInputVoiceStream(UdpSocketObject socket, Exception* exception)
{
	try
	{
		return new voice::InputVoice(*reinterpret_cast<web::UDPSocket*>(socket), frameSize, sampleRate, echoState, preprocessState);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return nullptr;
}

OutputVoiceStreamObject createOutputVoiceStream(UdpSocketObject socket, Exception* exception)
{
	try
	{
		return new voice::OutputVoice(*reinterpret_cast<web::UDPSocket*>(socket), frameSize, sampleRate, echoState, preprocessState);
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

int64_t ping(UdpSocketObject socket, Exception* exception)
{
	try
	{
		web::UDPSocket& udpSocket = *static_cast<web::UDPSocket*>(socket);

		udpSocket.sendData(web::UDPSocket::ping);

		auto start = std::chrono::high_resolution_clock::now();

		udpSocket.receiveData
		(
			[](const web::UDPSocket::Buffer& data, socklen_t size, const sockaddr_in& address, const web::UDPSocket& socket)
			{
				if (size == SOCKET_ERROR)
				{
#ifdef __LINUX__
					throw std::runtime_error(std::format("Can't send data: {}", strerror(errno)));
#else

					throw std::runtime_error(std::format("Can't send data: {}", WSAGetLastError()));
#endif
				}
			}
		);

		auto end = std::chrono::high_resolution_clock::now();

		return std::chrono::duration_cast<std::chrono::milliseconds>((end - start)).count();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0;
}

void overrideInputDeviceId(InputVoiceStreamObject inputStream, uint32_t id, Exception* exception)
{
	try
	{
		static_cast<voice::InputVoice*>(inputStream)->overrideDeviceId(id);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void muteOrUnmute(InputVoiceStreamObject inputStream, Exception* exception)
{
	try
	{
		functionality::muteOrUnmute(*static_cast<voice::InputVoice*>(inputStream));
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

bool isStreamRunning(InputVoiceStreamObject inputStream, Exception* exception)
{
	try
	{
		return static_cast<voice::InputVoice*>(inputStream)->isStreamRunning();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return false;
}

void restartInput(InputVoiceStreamObject inputStream, Exception* exception)
{
	try
	{
		static_cast<voice::InputVoice*>(inputStream)->restart();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void setInputVolume(InputVoiceStreamObject inputStream, double volume, Exception* exception)
{
	try
	{
		static_cast<voice::InputVoice*>(inputStream)->setVolume(volume);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

double getInputVolume(InputVoiceStreamObject inputStream, Exception* exception)
{
	try
	{
		return static_cast<voice::InputVoice*>(inputStream)->getVolume();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}

	return 0.0;
}

void overrideOutputDeviceId(OutputVoiceStreamObject outputStream, uint32_t id, Exception* exception)
{
	try
	{
		static_cast<voice::OutputVoice*>(outputStream)->overrideDeviceId(id);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void restartOutput(OutputVoiceStreamObject outputStream, Exception* exception)
{
	try
	{
		static_cast<voice::OutputVoice*>(outputStream)->restart();
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

void setOutputVolume(OutputVoiceStreamObject inputStream, double volume, Exception* exception)
{
	try
	{
		static_cast<voice::OutputVoice*>(inputStream)->setVolume(volume);
	}
	catch (const std::exception& e)
	{
		*exception = new std::runtime_error(e.what());
	}
}

double getOutputVolume(OutputVoiceStreamObject inputStream, Exception* exception)
{
	try
	{
		return static_cast<voice::OutputVoice*>(inputStream)->getVolume();
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

const char* getVersion()
{
	return functionality::getDiscordCloneClientLibraryVersion().data();
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

void deleteInputVoiceStream(InputVoiceStreamObject inputStream)
{
	delete static_cast<voice::InputVoice*>(inputStream);
}

void deleteOutputVoiceStream(OutputVoiceStreamObject outputStream)
{
	delete static_cast<voice::OutputVoice*>(outputStream);
}

void deleteException(Exception exception)
{
	delete static_cast<std::runtime_error*>(exception);
}

void deleteDeviceInformation(DeviceInformationArray deviceInformation)
{
	delete static_cast<std::vector<RtAudio::DeviceInfo>*>(deviceInformation);
}
