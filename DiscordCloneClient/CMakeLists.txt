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

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/Settings.cpp
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

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
