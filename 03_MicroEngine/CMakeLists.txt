cmake_minimum_required(VERSION 2.8.4)
project(engine)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/engine.cxx src/logger.cxx src/e_event.cxx nlohmann_json/json.hpp include/engine.hxx)

add_definitions(-DBUILD_ENGINE)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} "-lSDL2main -lSDL2")