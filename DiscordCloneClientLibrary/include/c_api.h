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
typedef void* InputVoiceStreamObject;
typedef void* OutputVoiceStreamObject;
typedef void* DeviceInformationArray;

CLIENT_LIBRARY_FUNCTION_API void initializeEchoCancelation(Exception* exception);

CLIENT_LIBRARY_FUNCTION_API UdpSocketObject createSocket(const char* ip, uint16_t port, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API InputVoiceStreamObject createInputVoiceStream(UdpSocketObject socket, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API OutputVoiceStreamObject createOutputVoiceStream(UdpSocketObject socket, Exception* exception);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLIENT_LIBRARY_FUNCTION_API void sendData(UdpSocketObject socket, const char* data, uint64_t size, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void receiveData(UdpSocketObject socket, void(*callback)(const char* data, uint64_t size, void* userData), int32_t flags, void* userData, Exception* exception);

/**
 * @brief Check ping to server. Needs separate connection
 * @param socket 
 * @param exception 
 * @return 
 */
CLIENT_LIBRARY_FUNCTION_API int64_t ping(UdpSocketObject socket, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void overrideInputDeviceId(InputVoiceStreamObject inputStream, uint32_t id, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void muteOrUnmute(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API bool isStreamRunning(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void restartInput(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void setInputVolume(InputVoiceStreamObject inputStream, double volume, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API double getInputVolume(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void overrideOutputDeviceId(OutputVoiceStreamObject outputStream, uint32_t id, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void restartOutput(OutputVoiceStreamObject outputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void setOutputVolume(OutputVoiceStreamObject inputStream, double volume, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API double getOutputVolume(OutputVoiceStreamObject inputStream, Exception* exception);

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

CLIENT_LIBRARY_FUNCTION_API void deleteInputVoiceStream(InputVoiceStreamObject inputStream);

CLIENT_LIBRARY_FUNCTION_API void deleteOutputVoiceStream(OutputVoiceStreamObject outputStream);

CLIENT_LIBRARY_FUNCTION_API void deleteException(Exception exception);

CLIENT_LIBRARY_FUNCTION_API void deleteDeviceInformation(DeviceInformationArray deviceInformation);
