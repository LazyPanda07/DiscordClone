cmake_minimum_required(VERSION 3.27.0)

set(RTAUDIO_TAG 409636b)
set(UTILITY_LIBRARY_TAG v0.11.0)
set(BUILD_SHARED_LIBS OFF)
set(RTAUDIO_BUILD_STATIC_LIBS ON)

FetchContent_Declare(
	rtaudio
	GIT_REPOSITORY https://github.com/thestk/rtaudio.git
	GIT_TAG ${RTAUDIO_TAG}
)

FetchContent_Declare(
	UtilityLibrary
	GIT_REPOSITORY https://github.com/LazyPanda07/UtilityLibrary.git
	GIT_TAG ${UTILITY_LIBRARY_TAG}
)

FetchContent_MakeAvailable(rtaudio)
FetchContent_MakeAvailable(UtilityLibrary)

project(DiscordClone)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/InputVoice.cpp
	src/OutputVoice.cpp
)

add_compile_options()

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	UDPSocket
	rtaudio
	UtilityLibrary
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
