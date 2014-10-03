project(OGLTG)
cmake_minimum_required(VERSION 2.8)

# packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV REQUIRED )

# source and header files
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} include)
aux_source_directory(src SRC_SRC)
aux_source_directory(. SRC_LIST)

# executable
add_executable(${PROJECT_NAME} ${SRC_SRC} ${SRC_LIST})

# libraries
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OpenCV_LIBS})

