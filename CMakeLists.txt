cmake_minimum_required(VERSION 3.20)

set(THIS "paint-gl")
project(${THIS} CXX)

set(CMAKE_CXX_STANDARD_REQUIRED 17)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

set(INCLUDES
   src/shaderf.glsl
   src/shaderv.glsl
   ${OPENGL_INCLUDE_DIRS}
   ${GLUT_INCLUDE_DIRS}
   ${GLEW_INCLUDE_DIRS}
)

set(LIBS
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARY}
    GLEW::glew
)

set(HEADERS
)

set(SOURCES
    "src/main.cpp"
)

message(${CMAKE_SOURCE_DIR})

link_directories(${INCLUDES})
add_executable(${THIS} ${HEADERS} ${SOURCES})
target_link_libraries(${THIS} ${LIBS})
