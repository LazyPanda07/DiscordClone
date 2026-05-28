cmake_minimum_required(VERSION 3.27.0)

set(RTAUDIO_TAG e5f0774)
set(UTILITY_LIBRARY_TAG v0.12.1)
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

if (${BUILD_SHARED_LIBS})
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
