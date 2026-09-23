cmake_minimum_required(VERSION 3.27.0)

if (${BUILD_SHARED_CLIENT_LIBRARY})
	set(LIBRARY_TYPE SHARED)

	add_definitions(-DLIBRARY_SIDE)
else()
	set(LIBRARY_TYPE STATIC)
endif()

project(DiscordCloneClientLibrary)

add_library(
	${PROJECT_NAME} ${LIBRARY_TYPE}
	src/Microphone.cpp
	src/Speaker.cpp
	src/Functionality.cpp
	src/c_api.cpp
)

if (WIN32)
	find_package(CUDAToolkit REQUIRED)

	target_link_libraries(
		${PROJECT_NAME} PRIVATE
		CUDA::cudart
		CUDA::cuda_driver
	)

	target_sources(${PROJECT_NAME} PRIVATE ${VIDEO_CODEC_SOURCES})
endif()

add_dependencies(${PROJECT_NAME} OpenCV::world)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	include
	${OPENCV_INSTALL_DIR}/include
	${VIDEO_CODEC_SDK_INCLUDE_DIRECTORY}
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${OPENCV_INSTALL_DIR}/lib
	${VIDEO_CODEC_SDK_LIB_DIRECTORY}
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	UDPSocket
	rtaudio
	UtilityLibrary
	opus
	Networks
	OpenCV::world
	${OPENCV_LIBRARIES}
	${VIDEO_CODEC_SDK_LIBRARIES}
)

if (WIN32)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
		winmm.lib
		DXCam
		${DXCAM_LIBRARIES}
	)
endif()

if (${BUILD_SHARED_CLIENT_LIBRARY})
	install(
		TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
	)
endif()

if (UNIX)
	set(TEMP ${VIDEO_CODEC_SDK_LIBRARIES})

	list(TRANSFORM TEMP PREPEND "${VIDEO_CODEC_SDK_LIB_DIRECTORY}/lib")
	list(TRANSFORM TEMP APPEND ".so")
	
	install(FILES ${TEMP} DESTINATION lib)
endif()
