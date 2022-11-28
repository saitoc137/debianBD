cmake_minimum_required(VERSION 3.21)
project(project1)
set(CMAKE_CXX_STANDARD 14)
add_executable(project1 main.cpp)
include_directories("E://alclion//untitled") #换成头文件所在目录


set(OpenCV_DIR "D:\\opencv-4.5.4\\opencv-4.5.4\\mingw-build\\install")
FIND_PACKAGE(OpenCV REQUIRED)
TARGET_LINK_LIBRARIES(project1 ${OpenCV_LIBS})



