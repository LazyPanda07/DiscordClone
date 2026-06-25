#pragma once

#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef SHARED_CLIENT_LIBRARY
#ifdef LIBRARY_SIDE
#ifdef __LINUX__
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __attribute__((visibility("default")))
#else
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __declspec(dllexport)
#endif
#else
#ifdef __LINUX__
#define CLIENT_LIBRARY_FUNCTION_API extern "C"
#else
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __declspec(dllimport)
#endif
#endif
#else
#define CLIENT_LIBRARY_FUNCTION_API
#endif

typedef void* Exception;
typedef void* UdpSocketObject;
typedef void* MicrophoneObject;
typedef void* SpeakerObject;
typedef void* DeviceInformationArray;

CLIENT_LIBRARY_FUNCTION_API void initialize(Exception* exception);

CLIENT_LIBRARY_FUNCTION_API UdpSocketObject createSocket(const char* ip, uint16_t port, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API MicrophoneObject createMicrophone(UdpSocketObject socket, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API SpeakerObject createSpeaker(UdpSocketObject socket, Exception* exception);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLIENT_LIBRARY_FUNCTION_API void sendData(UdpSocketObject socket, const char* data, uint64_t size, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void receiveData(UdpSocketObject socket, void(*callback)(const char* data, uint64_t size, void* userData), int32_t flags, void* userData, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void overrideMicrophoneDeviceId(MicrophoneObject microphone, uint32_t id, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void muteOrUnmute(MicrophoneObject microphone, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API bool isStreamRunning(MicrophoneObject microphone, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void restartMicrophone(MicrophoneObject microphone, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void setMicrophoneVolume(MicrophoneObject microphone, double volume, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API double getMicrophoneVolume(MicrophoneObject microphone, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void overrideSpeakerDeviceId(SpeakerObject speaker, uint32_t id, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void restartSpeaker(SpeakerObject speaker, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void setSpeakerVolume(SpeakerObject microphone, double volume, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API double getSpeakerVolume(SpeakerObject microphone, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API DeviceInformationArray getDeviceInformation(Exception* exception);

CLIENT_LIBRARY_FUNCTION_API uint64_t getDeviceInformationSize(DeviceInformationArray deviceInformation, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API uint32_t getDeviceInformationId(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API uint32_t getDeviceInformationInputChannels(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API uint32_t getDeviceInformationOutputChannels(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API bool getDeviceInformationDefaultInput(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API bool getDeviceInformationDefaultOutput(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API const char* getDeviceInformationName(DeviceInformationArray deviceInformation, uint64_t index, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API const char* getVersion();

CLIENT_LIBRARY_FUNCTION_API const char* getExceptionMessage(Exception exception);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLIENT_LIBRARY_FUNCTION_API void deleteSocket(UdpSocketObject socket);

CLIENT_LIBRARY_FUNCTION_API void deleteMicrophone(MicrophoneObject microphone);

CLIENT_LIBRARY_FUNCTION_API void deleteSpeaker(SpeakerObject speaker);

CLIENT_LIBRARY_FUNCTION_API void deleteException(Exception exception);

CLIENT_LIBRARY_FUNCTION_API void deleteDeviceInformation(DeviceInformationArray deviceInformation);
