cmake_minimum_required(VERSION 3.27.0)

set(RTAUDIO_TAG e5f0774)
set(UTILITY_LIBRARY_VERSION 0.12.1)
set(OPUS_VERSION 1.6.1)
set(RTAUDIO_BUILD_STATIC_LIBS ON)
set(OPUS_X86_PRESUME_AVX2 ON)

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

FetchContent_MakeAvailable(rtaudio)
FetchContent_MakeAvailable(UtilityLibrary)
FetchContent_MakeAvailable(opus)

if (${BUILD_SHARED_LIBS})
	add_definitions(-DLIBRARY_SIDE)
endif()

project(DiscordCloneClientLibrary)

add_library(
	${PROJECT_NAME}
	src/InputVoice.cpp
	src/OutputVoice.cpp
	src/Functionality.cpp
	src/c_api.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	include
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	UDPSocket
	rtaudio
	UtilityLibrary
	opus
)

if (WIN32)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
		winmm.lib
	)
endif()

if (${BUILD_SHARED_LIBS})
	install(
		TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
	)
endif()
