#!/usr/bin/env bash
SFML_BASE_DIR="./SFML-2.3.2-osx-clang-universal"
SFML_INCLUDE_PATH="-I$SFML_BASE_DIR/include"
SFML_LIBRARY_PATH="-L$SFML_BASE_DIR/lib"
SFML_LIBRARIES="-lsfml-audio -lsfml-graphics -lsfml-network -lsfml-system -lsfml-window"

CPP_FLAGS="-std=c++14"
INCLUDES="$SFML_INCLUDE_PATH"
LIBRARY_PATHS="$SFML_LIBRARY_PATH"
LIBRARIES="$SFML_LIBRARIES"
SOURCES="main.cpp"
OUTPUT_FILE="game_of_life"

g++ $CPP_FLAGS $INCLUDES $LIBRARY_PATHS $LIBRARIES $SOURCES -o $OUTPUT_FILE
