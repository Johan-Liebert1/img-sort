#!/bin/bash

mkdir -p bin

gcc -o bin/main main.c stb_image.h ./sort/*.c -lm -lSDL2 -lSDL2_image -lpthread

if [[ -z $1 ]]; then
    ./bin/main
fi
