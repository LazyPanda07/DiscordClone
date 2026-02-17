cmake_minimum_required(VERSION 3.27.0)

project(DiscordClone)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
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
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
