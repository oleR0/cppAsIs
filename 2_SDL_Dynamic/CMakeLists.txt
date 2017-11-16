cmake_minimum_required(VERSION 2.8.4)
project(SDL-Dynamic)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/sdl.cpp)
add_executable(SDL-Dynamic ${SOURCE_FILES})

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIR})
#target_link_libraries(2 ${SDL2_LIBRARIES}) it must work, but it doesn't...
#what is -mwindows???

target_link_libraries(SDL-Dynamic "-lSDL2main -lSDL2")
                        