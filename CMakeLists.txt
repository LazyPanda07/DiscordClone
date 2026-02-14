cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(FETCHCONTENT_QUIET OFF CACHE BOOL "")
option(BUILD_CLIENT "" ON)
option(BUILD_SERVER "" ON)

project(DiscordCloneProject)

include(FetchContent)

add_subdirectory(UDPSocket)

if (${BUILD_SERVER})
	add_subdirectory(Server)
endif()

if (${BUILD_CLIENT})
	add_subdirectory(Client)
endif()
