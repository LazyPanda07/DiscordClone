cmake_minimum_required(VERSION 4.0.0)

project(UDPSocket)

add_library(
	${PROJECT_NAME} STATIC
	src/UDPSocket.cpp
	src/UDPClientSocket.cpp
	src/UDPServerSocket.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	include
)

if (WIN32)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
		ws2_32
	)
endif()
