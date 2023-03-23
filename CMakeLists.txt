cmake_minimum_required(VERSION 3.9)
PROJECT(DHVis CXX)

set(CMAKE_CXX_STANDARD 14)

# OpenGL settings
set(OpenGL_GL_PREFERENCE LEGACY)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -ljsoncpp")
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# eigen
FIND_PACKAGE(Eigen3 REQUIRED Core)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIRS})

# jsoncpp
FIND_PACKAGE(jsoncpp REQUIRED)
# TODO: fix, add FindJsoncpp.cmake
INCLUDE_DIRECTORIES("/usr/include/jsoncpp")
SET(JSONCPP_LIBRARIES jsoncpp)


INCLUDE_DIRECTORIES(include)

add_executable(
        dh_vis
        src/main.cpp
        src/misc/models.cpp
        src/robot.cpp
        include/misc/models.h
        include/robot.h
        src/misc/camera.cpp
        include/misc/camera.h
        src/misc/glHelper.cpp
        include/misc/glHelper.h
)


TARGET_LINK_LIBRARIES(
        dh_vis
        ${CMAKE_THREAD_LIBS_INIT}
        ${JSONCPP_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
        -lGL
        -lglut
)
