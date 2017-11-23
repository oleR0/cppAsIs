cmake_minimum_required(VERSION 2.8.4)
project(game.exe)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/game.cxx)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} "-lengine.dll")
                        