cmake_minimum_required(VERSION 2.8.4)
project(SDL-Init)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/sdl.cpp)
add_executable(SDL-Init ${SOURCE_FILES})


if (MINGW)
	
	target_link_libraries(SDL-Init
               -Wl, SDL2.a
               -lm
               -ldinput8
               -ldxguid
               -ldxerr8
               -luser32 
               -lgdi32
               -lwinmm
               -limm32
               -lole32
               -loleaut32
               -lshell32
               -lversion
               -luuid
               -static-libgcc
               )
elseif(UNIX)
	find_library(SDL2_LIB libSDL2.a)
	target_link_libraries(${PROJECT_NAME} 
               -Wl,--no-undefined
			   "${SDL2_LIB}"
               -lm
               -ldl
               -lpthread
               -lrt
               )
endif()