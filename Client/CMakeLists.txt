cmake_minimum_required(VERSION 3.27.0)

set(RTAUDIO_TAG 409636b)
set(BUILD_SHARED_LIBS OFF)
set(RTAUDIO_BUILD_STATIC_LIBS ON)

FetchContent_Declare(
	rtaudio
	GIT_REPOSITORY https://github.com/thestk/rtaudio.git
	GIT_TAG ${RTAUDIO_TAG}
)

FetchContent_MakeAvailable(rtaudio)

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
	rtaudio
	UDPSocket
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
