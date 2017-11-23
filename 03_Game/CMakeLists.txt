cmake_minimum_required(VERSION 2.8.4)
project(game)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/game.cxx)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
link_directories(../../03_MicroEngine/build/)
target_link_libraries(${PROJECT_NAME} "-lengine")
                        