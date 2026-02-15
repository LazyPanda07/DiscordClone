cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "")
option(BUILD_CLIENT "" ON)
option(BUILD_SERVER "" ON)

project(DiscordCloneProject VERSION 0.1.0)

include(FetchContent)

if (WIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

add_subdirectory(UDPSocket)

if (${BUILD_SERVER})
	add_subdirectory(Server)
endif()

if (${BUILD_CLIENT})
	add_subdirectory(Client)
endif()

add_custom_target(
	build_docker_image
	WORKING_DIRECTORY docker
	COMMAND sudo docker build --build-arg BUILD_TAG=${PROJECT_VERSION} -t lazypanda07/discord_clone:${PROJECT_VERSION} .
)
