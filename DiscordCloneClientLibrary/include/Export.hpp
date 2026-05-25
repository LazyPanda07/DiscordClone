#pragma once

#ifdef EXPORT_CLIENT_LIBRARY
#ifdef __LINUX__
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __attribute__((visibility("default")))
#else
#define CLIENT_LIBRARY_FUNCTION_API extern "C" __declspec(dllexport)
#endif
#else
#define CLIENT_LIBRARY_FUNCTION_API
#endif
