cmake_minimum_required(VERSION 3.19)
project(stonewall VERSION 0.1)

#set(CMAKE_CXX_STANDARD 20)

string(TIMESTAMP BUILDDATE)
add_compile_options(-ggdb -Wno-deprecated-enum-enum-conversion -DVERSION="${CMAKE_PROJECT_VERSION}"  -DBUILDDATE="${BUILDDATE}")

add_executable(stonewall src/stonewall.cpp )

find_package(PkgConfig REQUIRED)

#Establishes the target install directory for any built libraries:
set(BUILD_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/external/usr")

#glm:
find_package(glm REQUIRED)
target_link_libraries(stonewall ${glm_LIBS} )

#opencv:
if (DEFINED BUILD_OPENCV)
	if (DEFINED HOST)
		set(CMAKECMD "${HOST}-cmake")
	else()
		set(CMAKECMD "cmake")
	endif()
	set(OPENCV_DEPS -lpng -lz)
	set(CMAKE_ARGUMENTS  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENCV_GENERATE_PKGCONFIG=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_opencv_apps=OFF -DOPENCV_TRACE=OFF -DWITH_IPP=OFF -DBUILD_LIST=core,imgproc,imgcodecs -DWITH_OPENCL=OFF -DWITH_PNG=ON -DWITH_JPEG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_JPEGXL=OFF -DWITH_AVIF=OFF -DWITHV4L=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_MSMF=OFF -DWITH_DSHOW=OFF -DWITH_AVFOUNDATION=OFF -DVIDEOIO_ENABLE_PLUGINS=OFF -DWITH_GTK=OFF -DWITH_WIN32UI=OFF -DHIGHGUI_ENABLE_PLUGINS=OFF -DWITH_PROTOBUF=OFF -DBUILD_PROTOBUF=OFF -DOPENCV_DNN_OPENCL=OFF -DENABLE_PRECOMPILED_HEADERS=OFF -DENABLE_PYLINT=OFF -DENABLE+FLAKE8=OFF -DBUILD_JAVA=OFF -DBUILD_FAT_JAVA_LIB=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DWITH_ITT=OFF -DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX} )
	#set(CMAKE_ARGUMENTS  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENCV_GENERATE_PKGCONFIG=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_opencv_apps=OFF -DBUILD_LIST=core,imgproc,imgcodecs -DWITH_OPENCL=OFF -DWITH_PNG=ON -DWITH_JPEG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_JPEGXL=OFF -DWITHV4L=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_MSMF=OFF -DWITH_DSHOW=OFF -DWITH_AVFOUNDATION=OFF -DVIDEOIO_ENABLE_PLUGINS=OFF -DWITH_GTK=OFF -DWITH_WIN32UI=OFF -DHIGHGUI_ENABLE_PLUGINS=OFF -DWITH_PROTOBUF=OFF -DBUILD_PROTOBUF=OFF -DOPENCV_DNN_OPENCL=OFF -DENABLE_PRECOMPILED_HEADERS=OFF -DENABLE_PYLINT=OFF -DENABLE+FLAKE8=OFF -DBUILD_JAVA=OFF -DBUILD_FAT_JAVA_LIB=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX} )

	include(ExternalProject)
	if (EXISTS ${BUILD_OPENCV})
		message(STATUS "Configuring opencv as an external project (local file: ${BUILD_OPENCV}).")
		ExternalProject_Add(opencv_download
			URL ${BUILD_OPENCV}
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
			PREFIX external/opencv
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	else ()
		message(FATAL_ERROR "BUILD_OPENCV not valid: ${BUILD_OPENCV} (Either GITHUB, SRCPKG, or valid local path/srcpackage")
	endif()

	set(OPENCV_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/include)
	set(OPENCV_INC_DIR ${LIBNOISE_INCLUDE_DIR} CACHE STRING "opencv include directory")
	set(OPENCV_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib)
	set(OPENCV_LIB   ${OPENCV_LIB_DIR}/libopencv_imgcodecs.a ${OPENCV_LIB_DIR}/libopencv_imgproc.a  ${OPENCV_LIB_DIR}/libopencv_core.a )
	
	add_library(opencv STATIC IMPORTED)
	set_target_properties(opencv PROPERTIES IMPORTED_LOCATION ${OPENCV_LIB_DIR} ) 
	set(OPENCV_FOUND TRUE)
	add_dependencies(stonewall opencv_download)
	
	target_include_directories(stonewall PRIVATE ${OPENCV_INCLUDE_DIR})
	target_link_libraries(stonewall ${OPENCV_LIB}  ${OPENCV_DEPS})
else()
	find_package( OpenCV REQUIRED )
	if (OpenCV_FOUND)
		include_directories( ${OpenCV_INCLUDE_DIRS} )
		target_link_libraries( stonewall ${OpenCV_LIBS} )
	else()
		message(FATAL_ERROR "OpenCV not found")
	endif(OpenCV_FOUND)
endif ()

#manifold:
if (DEFINED BUILD_MANIFOLD)
	if (DEFINED HOST)
		set(CMAKECMD "${HOST}-cmake")
	else()
		set(CMAKECMD "cmake")
	endif()
	set(MANIFOLD_DEPS -lpng -lz)
	set(CMAKE_ARGUMENTS -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DMANIFOLD_DEBUG=ON -DMANIFOLD_USE_BUILTIN_CLIPPER2=ON -DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX} )

	include(ExternalProject)
	if (EXISTS ${BUILD_MANIFOLD})
		message(STATUS "Configuring manifold as an external project (local file: ${BUILD_MANIFOLD}).")
		ExternalProject_Add(manifold_download
			URL ${BUILD_MANIFOLD}
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
			PREFIX external/manifold
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	else ()
		message(FATAL_ERROR "BUILD_MANIFOLD not valid: ${BUILD_MANIFOLD}" )
	endif()

	set(MANIFOLD_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/include)
	set(MANIFOLD_INC_DIR ${MANIFOLD_INCLUDE_DIR} CACHE STRING "manifold include directory" )
	set(MANIFOLD_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib)
	set(MANIFOLD_LIB ${MANIFOLD_LIB_DIR}/libmanifold.a ${MANIFOLD_LIB_DIR}/libmanifoldc.a ${MANIFOLD_LIB_DIR}/libclipper2.a)
	
	add_library(manifold STATIC IMPORTED)
	set_target_properties(manifold PROPERTIES IMPORTED_LOCATION ${MANIFOLD_LIB_DIR} ) 
	set(MANIFOLD_FOUND TRUE)
	add_dependencies(stonewall manifold_download)
	
	target_include_directories(stonewall PRIVATE ${MANIFOLD_INCLUDE_DIR})
	target_link_libraries(stonewall ${MANIFOLD_LIB}  ${MANIFOLD_DEPS})
else()
	find_package( Manifold REQUIRED )
	if (Manifold_FOUND)
		include_directories( ${Manifold_INCLUDE_DIRS} )
		target_link_libraries( stonewall ${Manifold_LIBS} )
	else()
		message(FATAL_ERROR "Manifold not found")
	endif(Manifold_FOUND)
endif ()

#libnoise:
if (DEFINED BUILD_LIBNOISE)
	if (DEFINED HOST)
		set(CMAKECMD "${HOST}-cmake")
	else()
		set(CMAKECMD "cmake")
	endif()
	set(CMAKE_ARGUMENTS -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX} )

	include(ExternalProject)
	if (BUILD_LIBNOISE STREQUAL "GITHUB")
		message(STATUS "Configuring libnoise as an external project (GitHub clone).")
		ExternalProject_Add(libnoise_download
			PREFIX external/libnoise
			GIT_REPOSITORY	  https://github.com/eXpl0it3r/libnoise.git
			GIT_PROGRESS      ON
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	elseif (BUILD_LIBNOISE STREQUAL "SRCPKG")
		message(STATUS "Configuring libnoise as an external project (URL download).")
		ExternalProject_Add(libnoise_download
			PREFIX external/libnoise
			#DOWNLOAD_DIR "external"
			URL https://github.com/eXpl0it3r/libnoise/archive/refs/heads/master.zip
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	elseif (EXISTS ${BUILD_LIBNOISE})
		message(STATUS "Configuring libnoise as an external project (local file: ${BUILD_LIBNOISE}).")
		ExternalProject_Add(libnoise_download
			URL ${BUILD_LIBNOISE}
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
			PREFIX external/libnoise
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	else ()
		message(FATAL_ERROR "BUILD_LIBNOISE not valid: ${BUILD_LIBNOISE} (Either GITHUB, SRCPKG, or valid local path/srcpackage")
	endif()

	set(LIBNOISE_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/include/noise)
	set(LIBNOISE_INC_DIR ${LIBNOISE_INCLUDE_DIR} CACHE STRING "libnoise include directory")
	set(LIBNOISE_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib)
	set(LIBNOISE_LIB ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib/libnoise.a)
	
	add_library(libnoise STATIC IMPORTED)
	set_target_properties(libnoise PROPERTIES IMPORTED_LOCATION ${LIBNOISE_LIB_DIR}/libnoise.a)
	set(LIBNOISE_FOUND TRUE)
	add_dependencies(stonewall libnoise_download)
	
	target_include_directories(stonewall PRIVATE ${LIBNOISE_INCLUDE_DIR})
	target_link_libraries(stonewall ${LIBNOISE_LIB}  ${LIBNOISE_DEPS})
else()
	find_package(LibNoise REQUIRED)
	if (LIBNOISE_FOUND)
		include_directories( ${LIBNOISE_INCLUDE_DIR} )
		target_link_libraries( stonewall ${LIBNOISE_LIBRARY} )
	else()
		message(FATAL_ERROR "libnoise not found")
	endif(LIBNOISE_FOUND)
endif ()



#target_link_libraries(stonewall -lassimp)

if(WIN32)
#	target_link_libraries(stonewall wsock32 ws2_32 psapi)
endif(WIN32)

add_subdirectory(src)

