#!/bin/bash

SFML_ROOT=../../../../libs/SFML-2.3-windows-gcc-4.9.2-mingw-32-bit/SFML-2.3
SFML_LIB=$SFML_ROOT/lib
SFML_INC=$SFML_ROOT/include

g++ tst_nvibros.cpp -L$SFML_LIB -I$SFML_INC -lsfml-system -lsfml-audio-d -o tst_nvibros.exe