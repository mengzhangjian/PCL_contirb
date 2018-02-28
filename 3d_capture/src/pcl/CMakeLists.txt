cmake_minimum_required( VERSION 2.8 )
project( test )

# 添加c++ 11标准支持
set( CMAKE_CXX_FLAGS "-std=c++11" )

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# find PCL lib
find_package(PCL 1.2 REQUIRED)
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( "/home/zhangjian/Desktop/3d_capture/src/include" )

link_directories("/home/zhangjian/Desktop/3d_capture/src/lib")

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable( test test.cpp )
# 链接OpenCV库
target_link_libraries( test ${OpenCV_LIBS} )
target_link_libraries (test ${PCL_LIBRARIES})
target_link_libraries( test "/home/zhangjian/Desktop/3d_capture/src/lib/libImiHandGrab.so")
target_link_libraries( test "/home/zhangjian/Desktop/3d_capture/src/lib/libiminect.so")
target_link_libraries( test "/home/zhangjian/Desktop/3d_capture/src/lib/libImiCamera.so")
target_link_libraries( test "/home/zhangjian/Desktop/3d_capture/src/lib/libjpeg.so.62")

