cmake_minimum_required(VERSION 3.27.0)

set(JSON_VERSION 3.4.2)

include(FetchContent)

FetchContent_Declare(
	JSON
	GIT_REPOSITORY https://github.com/LazyPanda07/JSON.git
	GIT_TAG v${JSON_VERSION}
)
FetchContent_MakeAvailable(JSON)

project(DiscordClone)

add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/Settings.cpp
	src/Utils.cpp
	src/Wrappers/SocketWrapper.cpp
	src/Wrappers/MicrophoneWrapper.cpp
	src/Wrappers/SpeakerWrapper.cpp
	src/Commands/Command.cpp
	src/Checks/Check.cpp
	src/Checks/CheckMicrophone.cpp
	src/Checks/CheckSpeaker.cpp
	src/Checks/CheckSocket.cpp
	src/Commands/Connect.cpp
	src/Commands/OverrideInputDeviceId.cpp
	src/Commands/OverrideOutputDeviceId.cpp
	src/Commands/SetMicrophoneVolume.cpp
	src/Commands/SetSpeakerVolume.cpp
	src/Commands/GetMicrophoneVolume.cpp
	src/Commands/GetSpeakerVolume.cpp
	src/Commands/MuteOrUnmute.cpp
	src/Commands/Echo.cpp
	src/Commands/Exit.cpp
)

if (WIN32)
	target_sources(
		${PROJECT_NAME} PRIVATE
		src/Platform/Windows/WindowsHotkeys.cpp
	)
else()
	target_sources(
		${PROJECT_NAME} PRIVATE
		src/Platform/Linux/LinuxHotkeys.cpp
	)
endif()

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	DiscordCloneClientLibrary
	JSON
)

if (WIN32)
	target_link_libraries(
		${PROJECT_NAME} PRIVATE
		OneCore
	)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
