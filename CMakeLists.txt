cmake_minimum_required(VERSION 2.8)
PROJECT( lec )
FIND_PACKAGE( OpenCV REQUIRED )
ADD_EXECUTABLE( lec lec.cpp )
TARGET_LINK_LIBRARIES( lec ${OpenCV_LIBS} )
SET(CMAKE_CXX_FLAGS "-std=c++0x")

