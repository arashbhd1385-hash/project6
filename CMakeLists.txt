cmake_minimum_required(VERSION 3.23)
project(sdl)

set(CMAKE_CXX_STANDARD 14)

add_executable(sdl main.cpp)
target_link_libraries(sdl -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
