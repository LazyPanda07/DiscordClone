cmake_minimum_required(VERSION 3.27.0)

project(3rdparty)

set(RTAUDIO_TAG e5f0774)
set(UTILITY_LIBRARY_VERSION 0.12.1)
set(OPUS_VERSION 1.6.1)
set(NETWORKS_VERSION 1.28.1)
set(WEB_FRAMEWORK_VERSION 3.4.2)
set(RTAUDIO_BUILD_STATIC_LIBS ON)
set(AVX2_SUPPORTED ON)
set(OPUS_X86_MAY_HAVE_AVX2 ON)
set(OPUS_X86_PRESUME_AVX2 ON)
set(SSE4_1_SUPPORTED ON)
set(OPUS_X86_MAY_HAVE_SSE4_1 ON)
set(OPUS_X86_PRESUME_SSE4_1 ON)
set(WEB_FRAMEWORK_URL https://github.com/LazyPanda07/WebFramework/releases/download/v${WEB_FRAMEWORK_VERSION})

if (UNIX)
    set(WEB_FRAMEWORK_URL ${WEB_FRAMEWORK_URL}/Linux-WITH_PYTHON_EXECUTORS.OFF-BUILD_PYTHON_API.OFF--WITH_DOTNET_EXECUTORS.OFF-BUILD_CSHARP_API.OFF-BUILD_CC_API.OFF.zip)
else()
    set(WEB_FRAMEWORK_URL ${WEB_FRAMEWORK_URL}/Windows-WITH_PYTHON_EXECUTORS.OFF-BUILD_PYTHON_API.OFF--WITH_DOTNET_EXECUTORS.OFF-BUILD_CSHARP_API.OFF-BUILD_CC_API.OFF.zip)
endif()

FetchContent_Declare(
	rtaudio
	GIT_REPOSITORY https://github.com/thestk/rtaudio.git
	GIT_TAG ${RTAUDIO_TAG}
)

FetchContent_Declare(
	UtilityLibrary
	GIT_REPOSITORY https://github.com/LazyPanda07/UtilityLibrary.git
	GIT_TAG v${UTILITY_LIBRARY_VERSION}
)

FetchContent_Declare(
	opus
	GIT_REPOSITORY https://gitlab.xiph.org/xiph/opus.git
	GIT_TAG v${OPUS_VERSION}
)

FetchContent_Declare(
	Networks
	GIT_REPOSITORY https://github.com/LazyPanda07/Networks.git
	GIT_TAG v${NETWORKS_VERSION}
)

FetchContent_Declare(
	WebFramework
	URL ${WEB_FRAMEWORK_URL}
)

FetchContent_MakeAvailable(rtaudio)
FetchContent_MakeAvailable(UtilityLibrary)
FetchContent_MakeAvailable(opus)
FetchContent_MakeAvailable(Networks)
FetchContent_MakeAvailable(WebFramework)

list(APPEND CMAKE_PREFIX_PATH ${webframework_SOURCE_DIR})

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
