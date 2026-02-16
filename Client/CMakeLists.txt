cmake_minimum_required(VERSION 3.27.0)

project(DiscordClone)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	ClientLibrary
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
