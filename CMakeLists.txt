CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(landmarker)


## opencv ----------------------------------------------------------------------
FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES(${OpenCV_HEADERS})

## source code -----------------------------------------------------------------
SET(PATH_INC ${landmarker_SOURCE_DIR}/include)
SET(PATH_SRC ${landmarker_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PATH_INC})
AUX_SOURCE_DIRECTORY(${PATH_SRC} SRC_LIST)

## build and link --------------------------------------------------------------
ADD_EXECUTABLE(landmarker ${SRC_LIST})
TARGET_LINK_LIBRARIES(landmarker ${OpenCV_LIBS})
