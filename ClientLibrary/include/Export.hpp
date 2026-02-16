#pragma once

#ifdef EXPORT_CLIENT_LIBRARY
#ifdef _WIN32
#define CLIENT_LIBRARY_API __declspec(dllexport)
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __declspec(dllexport)
#else
#define CLIENT_LIBRARY_API __attribute__((visibility("default")))
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __attribute__((visibility("default")))
#endif
#else
#define CLIENT_LIBRARY_API
#define CLIENT_LIBRARY_FUNCTION_API
#endif
