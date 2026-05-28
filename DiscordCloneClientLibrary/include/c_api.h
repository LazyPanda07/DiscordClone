#pragma once

#include <stdint.h>

#ifdef EXPORT_CLIENT_LIBRARY
#ifdef __LINUX__
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __attribute__((visibility("default")))
#else
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __declspec(dllexport)
#endif
#else
#define CLIENT_LIBRARY_FUNCTION_API
#endif

typedef void* Exception;
typedef void* InputVoiceStreamObject;
typedef void* OutputVoiceStreamObject;
typedef void* UdpSocketObject;

CLIENT_LIBRARY_FUNCTION_API UdpSocketObject createSocket(const char* ip, uint16_t port, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API InputVoiceStreamObject createInputVoiceStream(UdpSocketObject socket, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API OutputVoiceStreamObject createOutputVoiceStream(UdpSocketObject socket, Exception* exception);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLIENT_LIBRARY_FUNCTION_API void sendData(UdpSocketObject socket, const char* data, uint64_t size, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void receiveData(UdpSocketObject socket, void(*callback)(const char* data, uint64_t size, void* userData), void* userData, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void overrideInputDeviceId(InputVoiceStreamObject inputStream, uint32_t id, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void muteOrUnmute(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void restartInput(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void setInputVolume(InputVoiceStreamObject inputStream, double volume, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API double getInputVolume(InputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void overrideOutputDeviceId(OutputVoiceStreamObject outputStream, uint32_t id, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void restartOutput(OutputVoiceStreamObject outputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API void setOutputVolume(OutputVoiceStreamObject inputStream, double volume, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API double getOutputVolume(OutputVoiceStreamObject inputStream, Exception* exception);

CLIENT_LIBRARY_FUNCTION_API const char* getVersion();

CLIENT_LIBRARY_FUNCTION_API void printDeviceInfo(Exception* exception);

CLIENT_LIBRARY_FUNCTION_API const char* getExceptionMessage(Exception exception);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLIENT_LIBRARY_FUNCTION_API void deleteSocket(UdpSocketObject socket);

CLIENT_LIBRARY_FUNCTION_API void deleteInputVoiceStream(InputVoiceStreamObject inputStream);

CLIENT_LIBRARY_FUNCTION_API void deleteOutputVoiceStream(OutputVoiceStreamObject outputStream);

CLIENT_LIBRARY_FUNCTION_API void deleteException(Exception exception);
