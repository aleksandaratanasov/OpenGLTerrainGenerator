project(OGLTG)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
#find_package(Threads)
# source and header files
aux_source_directory(src SRC_SRC)
#aux_source_directory(IO SRC_IO)
#aux_source_directory(Scanner SRC_SCANNER)
#aux_source_directory(Parser SRC_PARSER)
#add_executable(${PROJECT_NAME} ${SRC_CONTAINER} ${SRC_IO} ${SRC_SCANNER} ${SRC_PARSER} main.cpp)
#target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
add_executable(${PROJECT_NAME} ${SRC_SRC} ${SRC_LIST})


