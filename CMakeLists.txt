cmake_minimum_required(VERSION 3.23)



project(project6)

set(CMAKE_CXX_STANDARD 14)

add_executable(project6 main.cpp)
target_link_libraries(project6 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)

