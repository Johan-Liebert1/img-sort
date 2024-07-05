#!/bin/zsh

mkdir -p bin

gcc -o bin/main main.c stb_image.h -lm -lSDL2 -lSDL2_image

if [[ -z $1 ]]; then
    ./bin/main
fi
