cmake_minimum_required(VERSION 3.27.0)

project(3rdparty)

set(RTAUDIO_TAG e5f0774)
set(OPUS_VERSION 1.6.1)
set(WEB_FRAMEWORK_VERSION 3.4.2)
set(MINIZ_VERSION 3.1.1)
set(RTAUDIO_BUILD_STATIC_LIBS ON)
set(AVX2_SUPPORTED ON)
set(OPUS_X86_MAY_HAVE_AVX2 ON)
set(OPUS_X86_PRESUME_AVX2 ON)
set(SSE4_1_SUPPORTED ON)
set(OPUS_X86_MAY_HAVE_SSE4_1 ON)
set(OPUS_X86_PRESUME_SSE4_1 ON)

set(BUILD_PYTHON_API OFF)
set(BUILD_CSHARP_API OFF)
set(WITH_PYTHON_EXECUTORS OFF)
set(WITH_DOTNET_EXECUTORS OFF)

FetchContent_Declare(
	rtaudio
	GIT_REPOSITORY https://github.com/thestk/rtaudio.git
	GIT_TAG ${RTAUDIO_TAG}
)

FetchContent_Declare(
	opus
	GIT_REPOSITORY https://github.com/xiph/opus.git
	GIT_TAG v${OPUS_VERSION}
)

FetchContent_Declare(
	WebFramework
	GIT_REPOSITORY https://github.com/LazyPanda07/WebFramework.git
	GIT_TAG v${WEB_FRAMEWORK_VERSION}
)

FetchContent_Declare(
	miniz
	GIT_REPOSITORY https://github.com/richgel999/miniz.git
	GIT_TAG ${MINIZ_VERSION}
)

FetchContent_MakeAvailable(rtaudio)
FetchContent_MakeAvailable(opus)
FetchContent_MakeAvailable(WebFramework)
FetchContent_MakeAvailable(miniz)
