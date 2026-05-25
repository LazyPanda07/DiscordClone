cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 23)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "")
option(BUILD_CLIENT "Build DiscordClone" ON)
option(BUILD_CLIENT_LIBRARY "Build DiscordCloneClientLibrary" ON)
option(BUILD_SERVER "Build DiscordCloneServer" ON)

project(DiscordCloneProject VERSION 0.2.0)

include(FetchContent)

if (WIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)

	if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
	endif()
endif()

add_subdirectory(UDPSocket)

if (${BUILD_SERVER})
	add_subdirectory(Server)
endif()

if (${BUILD_CLIENT_LIBRARY})
	if (${BUILD_CLIENT})
		set(BUILD_SHARED_CLIENT_LIBRARY OFF)
		add_definitions(-DEXPORT_CLIENT_LIBRARY)
	else()
		set(BUILD_SHARED_CLIENT_LIBRARY ON)
		set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	endif()

	add_subdirectory(DiscordCloneClientLibrary)
endif()

if (${BUILD_CLIENT})
	add_subdirectory(DiscordCloneClient)
endif()

add_custom_target(
	build_docker_image
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docker
	COMMAND sudo docker build --build-arg BUILD_TAG=v${PROJECT_VERSION} -t lazypanda07/discord_clone_server:v${PROJECT_VERSION} .
)
