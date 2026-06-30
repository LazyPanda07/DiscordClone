cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)

project(DiscordCloneUpdater)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/Releases.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	Networks
	UtilityLibrary
	miniz
	ssl
	crypto
)

if (WIN32)
	target_link_libraries(
		${PROJECT_NAME} PRIVATE
		crypt32
	)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
