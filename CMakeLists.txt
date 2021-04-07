cmake_minimum_required(VERSION 2.8)

project(OpenGLProject)
add_executable(OpenGLProject/OpenGLProject/joh29_animation OpenGLProject/OpenGLProject/main.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(OpenGLProject/OpenGLProject/joh29_animation ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
