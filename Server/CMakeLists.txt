cmake_minimum_required(VERSION 3.27.0)

project(DiscordCloneServer)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

install(TARGETS ${PROJECT_NAME} DESTINATION server/bin)
