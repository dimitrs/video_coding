# --------------------------------------------------------------
# Top level project
# --------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
PROJECT( video_coding )

ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)

SET(BUILD_SHARED_LIBS OFF)

#-----------------------------------
# SUBDIRECTORIES:
# Save libs and executables in the same place
#-----------------------------------
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin" CACHE PATH "Output directory for applications" )

# List of all .cpp files
SET(SRCS
  ./test.cpp	
)
  
SET(HDRS
	./cap_ffmpeg_impl.hpp
)

FIND_PACKAGE( OpenCV REQUIRED )

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}"    
    "${PROJECT_SOURCE_DIR}/ffmpeg-git-ceb0dd9-win32-dev/include"  
    )
    
LINK_DIRECTORIES(
		"${PROJECT_SOURCE_DIR}/ffmpeg-git-ceb0dd9-win32-dev/lib"
	)

ADD_EXECUTABLE( video_coding ${SRCS} )
TARGET_LINK_LIBRARIES( video_coding ${OpenCV_LIBS} avutil avformat avfilter avdevice avcodec swscale swresample postproc)

