cmake_minimum_required(VERSION 3.27.0)

project(DiscordCloneClientResources)

add_library(
	${PROJECT_NAME} SHARED
	src/Resources.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

if (WIN32)
	install(
		CODE [[
			file(REMOVE ${CMAKE_INSTALL_PREFIX}/bin/DiscordCloneClientResources.lib)
		]]
	)
endif()
