cmake_minimum_required(VERSION 3.27.0)

if (${BUILD_SHARED_CLIENT_LIBRARY})
	set(LIBRARY_TYPE SHARED)

	add_definitions(-DLIBRARY_SIDE)
else()
	set(LIBRARY_TYPE STATIC)
endif()

project(DiscordCloneClientLibrary)

add_library(
	${PROJECT_NAME} ${LIBRARY_TYPE}
	src/Microphone.cpp
	src/Speaker.cpp
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
	Networks
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
