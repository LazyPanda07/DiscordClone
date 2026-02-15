cmake_minimum_required(VERSION 3.27.0)

project(DiscordCloneServer)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/VoiceServer.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	include
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	UDPSocket
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
