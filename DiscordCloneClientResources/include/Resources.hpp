#pragma once

#include <stdint.h>

#ifdef __LINUX__
#define RESOURCES_FUNCTION_API extern "C" __attribute__((visibility("default")))
#else
#define RESOURCES_FUNCTION_API extern "C" __declspec(dllexport)
#endif

RESOURCES_FUNCTION_API const uint8_t* getMicrophoneOffSound(uint64_t* size);

RESOURCES_FUNCTION_API const uint8_t* getMicrophoneOnSound(uint64_t* size);
