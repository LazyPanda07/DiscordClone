cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 23)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "")
option(BUILD_CLIENT "Build DiscordClone" ON)
option(BUILD_CLIENT_LIBRARY "Build DiscordCloneClientLibrary" OFF)
option(BUILD_SERVER "Build DiscordCloneServer" ON)

project(DiscordCloneProject VERSION 0.2.0)

include(FetchContent)

if (WIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)

	if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
	endif()
endif()

if (UNIX)
	add_definitions(-D__LINUX__)
endif()

add_subdirectory(UDPSocket)

if (${BUILD_SERVER})
	add_subdirectory(Server)
endif()

if (${BUILD_CLIENT})
	add_subdirectory(DiscordCloneClientLibrary)

	add_subdirectory(DiscordCloneClient)
elseif(${BUILD_CLIENT_LIBRARY})
	add_subdirectory(DiscordCloneClientLibrary)
endif()

add_custom_target(
	build_docker_image
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docker
	COMMAND sudo docker build --build-arg BUILD_TAG=v${PROJECT_VERSION} -t lazypanda07/discord_clone_server:v${PROJECT_VERSION} .
)
