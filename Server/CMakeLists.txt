cmake_minimum_required(VERSION 3.27.0)

project(DiscordCloneServer)

find_package(WebFramework REQUIRED)

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
	WebFramework::CXX_API
)

if (UNIX)
	target_link_options(
		${PROJECT_NAME} PRIVATE
		-rdynamic
	)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

if (UNIX)
	install(
		CODE [[
			file(GLOB WEB_FRAMEWORK_LIBRARIES ${CMAKE_INSTALL_PREFIX}/*.so)

			foreach(LIBRARY IN LISTS WEB_FRAMEWORK_LIBRARIES)
				file(COPY ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
			endforeach()
		]]
	)
else()
	install(
		CODE [[
			file(GLOB WEB_FRAMEWORK_LIBRARIES ${CMAKE_INSTALL_PREFIX}/*.dll)

			foreach(LIBRARY IN LISTS WEB_FRAMEWORK_LIBRARIES)
				file(COPY ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
			endforeach()
		]]
	)
endif()
