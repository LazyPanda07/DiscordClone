cmake_minimum_required(VERSION 3.27.0)

project(3rdparty)

if (${BUILD_CLIENT} OR ${BUILD_CLIENT_LIBRARY})
	set(RTAUDIO_TAG e5f0774)
	set(OPUS_VERSION 1.6.1)
	set(MINIZ_VERSION 3.1.1)
	set(RTAUDIO_BUILD_STATIC_LIBS ON)
	set(AVX2_SUPPORTED ON)
	set(OPUS_X86_MAY_HAVE_AVX2 ON)
	set(OPUS_X86_PRESUME_AVX2 ON)
	set(SSE4_1_SUPPORTED ON)
	set(OPUS_X86_MAY_HAVE_SSE4_1 ON)
	set(OPUS_X86_PRESUME_SSE4_1 ON)

	FetchContent_Declare(
		rtaudio
		GIT_REPOSITORY https://github.com/thestk/rtaudio.git
		GIT_TAG ${RTAUDIO_TAG}
	)
	FetchContent_Declare(
		miniz
		GIT_REPOSITORY https://github.com/richgel999/miniz.git
		GIT_TAG ${MINIZ_VERSION}
	)
	FetchContent_Declare(
		opus
		GIT_REPOSITORY https://github.com/xiph/opus.git
		GIT_TAG v${OPUS_VERSION}
	)

	FetchContent_MakeAvailable(rtaudio)
	FetchContent_MakeAvailable(opus)
	FetchContent_MakeAvailable(miniz)

	if (NOT ${BUILD_SERVER})
		set(NETWORKS_VERSION 1.28.1)
		set(UTILITY_LIBRARY_VERSION 0.12.1)

		FetchContent_Declare(
			Networks
			GIT_REPOSITORY https://github.com/LazyPanda07/Networks.git
			GIT_TAG v${NETWORKS_VERSION}
		)
		FetchContent_Declare(
			UtilityLibrary
			GIT_REPOSITORY https://github.com/LazyPanda07/UtilityLibrary.git
			GIT_TAG v${UTILITY_LIBRARY_VERSION}
		)

		FetchContent_MakeAvailable(Networks)
		FetchContent_MakeAvailable(UtilityLibrary)
	endif()
endif()

if (${BUILD_SERVER})
	set(WEB_FRAMEWORK_VERSION 3.4.2)
	set(BUILD_PYTHON_API OFF)
	set(BUILD_CSHARP_API OFF)
	set(WITH_PYTHON_EXECUTORS OFF)
	set(WITH_DOTNET_EXECUTORS OFF)

	FetchContent_Declare(
		WebFramework
		GIT_REPOSITORY https://github.com/LazyPanda07/WebFramework.git
		GIT_TAG v${WEB_FRAMEWORK_VERSION}
	)

	FetchContent_MakeAvailable(WebFramework)
endif()

if (WIN32)
	if (${BUILD_CLIENT} OR ${BUILD_CLIENT_LIBRARY})
		include(ExternalProject)

		set(OPENCV_VERSION 5.0.0)
		set(DXCAM_VERSION 0.2.5)
		set(DXCAM_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/3rdparty/DXCam)

		if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			set(OPENCV_LIBRARY_NAME opencv_world500d)
			set(DXCAM_LIBRARY_NAME DXCamd)
		else()
			set(OPENCV_LIBRARY_NAME opencv_world500)
			set(DXCAM_LIBRARY_NAME DXCam)
		endif()

		add_library(OpenCV::world STATIC IMPORTED GLOBAL)
		add_library(DXCam STATIC IMPORTED GLOBAL)

		set_target_properties(
			OpenCV::world PROPERTIES
		    IMPORTED_LOCATION
		        "${OPENCV_INSTALL_DIR}/lib/${OPENCV_LIBRARY_NAME}.lib"
		
		    INTERFACE_INCLUDE_DIRECTORIES
		        "${OPENCV_INSTALL_DIR}/include"
		)
		set_target_properties(
			DXCam PROPERTIES
		    IMPORTED_LOCATION
		        "${DXCAM_INSTALL_DIR}/lib/${DXCAM_LIBRARY_NAME}.lib"
		
		    INTERFACE_INCLUDE_DIRECTORIES
		        "${DXCAM_INSTALL_DIR}/include"
		)

		file(MAKE_DIRECTORY "${OPENCV_INSTALL_DIR}/include")
		file(MAKE_DIRECTORY "${DXCAM_INSTALL_DIR}/include")

		if (NOT EXISTS ${OPENCV_INSTALL_DIR}/lib/${OPENCV_LIBRARY_NAME}.lib)
			ExternalProject_Add(
				external_opencv
				GIT_REPOSITORY https://github.com/opencv/opencv.git
				GIT_TAG ${OPENCV_VERSION}
				
				LOG_BUILD OFF

				CMAKE_ARGS
					-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
					-DCMAKE_INSTALL_PREFIX=${OPENCV_INSTALL_DIR}
					-DBUILD_SHARED_LIBS=OFF
					-DOPENCV_LIB_ARCHIVE_INSTALL_PATH=lib
					-DOPENCV_3P_LIB_INSTALL_PATH=lib
					-DBUILD_opencv_world=ON
					-DBUILD_opencv_python3=OFF
					-DBUILD_opencv_java=OFF
					-DBUILD_JAVA=OFF
					-DBUILD_opencv_js=OFF
					-DBUILD_opencv_objc=OFF
					-DBUILD_opencv_apps=OFF
					-DBUILD_TESTS=OFF
					-DBUILD_PERF_TESTS=OFF
					-DBUILD_EXAMPLES=OFF
					-DBUILD_DOCS=OFF
					-DBUILD_WITH_STATIC_CRT=OFF
					-G "Ninja"
			
				BUILD_BYPRODUCTS
					${OPENCV_INSTALL_DIR}/lib/${OPENCV_LIBRARY_NAME}.lib
			)

			add_dependencies(OpenCV::world external_opencv)
		endif()

		if (NOT EXISTS ${DXCAM_INSTALL_DIR}/lib/${DXCAM_LIBRARY_NAME}.lib)
			ExternalProject_Add(
				external_DXCam
				GIT_REPOSITORY https://github.com/Fidelxyz/DXCam-CPP.git
				GIT_TAG v${DXCAM_VERSION}
				DEPENDS OpenCV::world
			
				CMAKE_ARGS
					-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
					-DOpenCV_DIR=${OPENCV_INSTALL_DIR}
					-DCMAKE_INSTALL_PREFIX=${DXCAM_INSTALL_DIR}
					-G "Ninja"
			
				BUILD_BYPRODUCTS
					${DXCAM_INSTALL_DIR}/lib/${DXCAM_LIBRARY_NAME}.lib
			)

			add_dependencies(DXCam external_DXCam)
		endif()

	endif()
endif()
