#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIN_ASPECT_RATIO_FACTOR 100

int png_to_ppm() {
    int channels, width, height;
    stbi_uc *img_data = stbi_load("./tux.png", &width, &height, &channels, 3);
    printf("channels: %d, width: %d, height: %d\n", channels, width, height);

    int fd = open("./rgba.ppm", O_RDWR | O_TRUNC, 0666);

    if (fd < 0) {
        perror("open");
        printf("fd: %d\n", fd);
        return fd;
    }

    char buf[1024];
    int n = sprintf(buf, "P3\n%d %d\n255\n", width, height);

    write(fd, buf, n);

    for (int i = 0; i < width * height * channels; i += channels) {
        n = sprintf(buf, "%d %d %d\n", img_data[i], img_data[i + 1],
                    img_data[i + 2]);

        write(fd, buf, n);
    }

    return 0;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed! %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow("Mah window", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 16 * WIN_ASPECT_RATIO_FACTOR,
                         9 * WIN_ASPECT_RATIO_FACTOR, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);


    int height, width;

    SDL_Texture *image = IMG_LoadTexture(renderer, "./tux.png");
    SDL_QueryTexture(image, NULL, NULL, &width,
                     &height); // get the width and height of the texture

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;

                case SDL_KEYDOWN: {
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE: {
                            quit = 1;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, image, NULL,
                       &(SDL_Rect){.x = 0, .y = 0, .w = width, .h = height});
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
