cmake_minimum_required(VERSION 3.20)

project (Tetris LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

add_executable(Tetris 
    main.cpp
    game.cpp
    pieces.cpp
)

target_link_libraries(Tetris PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)