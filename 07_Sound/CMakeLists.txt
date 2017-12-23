cmake_minimum_required(VERSION 3.2.2)
project(07_Sound)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

include_directories("engine/include/")
include_directories("dependencies/")


file(GLOB SOURCE_FILES_ENGINE
    "engine/src/*"
    "engine/include/*"
    "dependencies/*"
)


add_library(engine SHARED ${SOURCE_FILES_ENGINE})
target_compile_features(engine PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine PRIVATE "-DTME_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine 
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine
               -lSDL2
               -lGL
               )
endif()

include_directories("engine/include/")
include_directories("game/include/")

file(GLOB SOURCE_FILES_GAME
    "game/src/*"
    "game/include/*"
)

add_executable(game ${SOURCE_FILES_GAME})
target_compile_features(game PUBLIC cxx_std_17)

target_link_libraries(game engine)
