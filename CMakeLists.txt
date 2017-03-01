cmake_minimum_required(VERSION 3.6)
project(Minesweeper)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Utils/Utils.h Menu/Menu.h Menu/Menu.cpp Utils/Utils.cpp Editor/Editor.h Editor/Editor.cpp Map/Map.h Map/Map.cpp Ranking/Ranking.h Ranking/Ranking.cpp)
add_executable(Minesweeper ${SOURCE_FILES})

#INCLUDE_DIRECTORIES(allegro/5.0.11/include)

#LINK_DIRECTORIES(allegro/5.0.11/lib)


TARGET_LINK_LIBRARIES(Minesweeper allegro_acodec
        allegro_audio
        allegro_color
        allegro_dialog
        allegro_image
        allegro_main
        allegro_memfile
        allegro_physfs
        allegro_primitives
        allegro_ttf
        allegro_font
        allegro)