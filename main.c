#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIN_ASPECT_RATIO_FACTOR 100
#define STRIP_WIDTH 50 // 20 px

void paint_image_strip(SDL_Renderer *renderer, stbi_uc *img_data,
                       int strip_number, int img_height, int img_width) {

    int num_iterations = STRIP_WIDTH * img_height;

    int i = STRIP_WIDTH * img_height * strip_number * 3;

    int x = STRIP_WIDTH * strip_number;
    int y = 0;

    for (int iter = 0; iter <= num_iterations; iter++) {
        SDL_SetRenderDrawColor(renderer, img_data[i], img_data[i + 1],
                               img_data[i + 2], 255);

        SDL_RenderDrawPoint(renderer, x, y);

        if (iter != 0 && iter % STRIP_WIDTH == 0) {
            x = STRIP_WIDTH * strip_number;
            y += 1;

            i = STRIP_WIDTH * img_height * strip_number * 3 + img_width * 3 * y;

            // printf("i: %d\n", i);

            continue;
        }

        x += 1;
        i += 3;
        // printf("i: %d\n", i);
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed! %s\n", SDL_GetError());
        return 1;
    }

    int channels, img_width, img_height;
    stbi_uc *img_data =
        stbi_load("./testimage.jpg", &img_width, &img_height, &channels, 3);

    printf("channels: %d, img_width: %d, img_height: %d\n", channels, img_width,
           img_height);

    SDL_Window *window = SDL_CreateWindow(
        "Mah window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        img_width, img_height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        paint_image_strip(renderer, img_data, 0, img_height, img_width);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
