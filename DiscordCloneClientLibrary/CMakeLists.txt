cmake_minimum_required(VERSION 3.27.0)

set(RTAUDIO_TAG e5f0774)
set(UTILITY_LIBRARY_TAG v0.12.0)
set(BUILD_SHARED_LIBS OFF)
set(RTAUDIO_BUILD_STATIC_LIBS ON)
set(LIBRARY_TYPE STATIC)

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

project(DiscordCloneClientLibrary)

if (${BUILD_SHARED_CLIENT_LIBRARY})
	set(LIBRARY_TYPE SHARED)
endif()

add_library(
	${PROJECT_NAME} ${LIBRARY_TYPE}
	src/InputVoice.cpp
	src/OutputVoice.cpp
	src/Functionality.cpp
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
)

if (WIN32)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
		winmm.lib
	)
endif()

if (${BUILD_SHARED_CLIENT_LIBRARY})
	install(
		TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
	)
endif()
