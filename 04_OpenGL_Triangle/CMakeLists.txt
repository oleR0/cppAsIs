cmake_minimum_required(VERSION 2.8.4)
project(game)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES_EXE src/game.cxx)
set(SOURCE_FILES_ENGINE src/engine.cxx nlohmann_json/json.hpp include/engine.hxx)

add_definitions(-DBUILD_ENGINE)
add_library(engine SHARED ${SOURCE_FILES_ENGINE})
target_include_directories(engine PRIVATE include)

if (MINGW)
    target_link_libraries(engine "-lSDL2main -lSDL2 -lopengl32")
elseif(UNIX)
    target_link_libraries(engine
               -lSDL2
               -lGL
               )
endif()
    

add_executable(${PROJECT_NAME} ${SOURCE_FILES_EXE})
target_include_directories(${PROJECT_NAME} PRIVATE include)
#link_directories(../../03_MicroEngine/build/)
target_link_libraries(${PROJECT_NAME} engine)
                   
                   

     