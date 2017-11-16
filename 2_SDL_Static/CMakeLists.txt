cmake_minimum_required(VERSION 2.8.4)
project(SDL-Static)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/sdl.cpp)
add_executable(SDL-Static ${SOURCE_FILES})


if (MINGW)
	#set(CMAKE_PREFIX_PATH c:/mingw-w64/mingw64/x86_64-w64-mingw32/lib)
	#find_library(SDL2_LIB libSDL2.a)

	target_link_libraries(SDL-Static
               -Wl, SDL2.a
               #-lm
               #-ldinput8
               #-ldxguid
               #-ldxerr8
               #-luser32 
               #-lgdi32
               -lwinmm
               -limm32
               #-lole32
               #-loleaut32
               #-lshell32
               -lversion
               #-luuid
               #-static-libgcc
               )
elseif(UNIX)
	find_library(SDL2_LIB libSDL2.a)
	target_link_libraries(SDL-Static
				-Wl, SDL2.a
				-ldl
				)
endif()