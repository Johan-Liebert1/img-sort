#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "sort/sort.h"

#define WIN_ASPECT_RATIO_FACTOR 100
#define STRIP_WIDTH 25
#define ANIMATION_DELAY_MS 50

void paint_image_strip(SDL_Renderer *renderer, Image* image,
                       int img_strip_number, int window_strip_number) {

    int num_iterations = STRIP_WIDTH * image->height;

    int i = STRIP_WIDTH * img_strip_number * 3;

    int x = STRIP_WIDTH * window_strip_number;
    int y = 0;

    for (int iter = 0; iter <= num_iterations; iter++) {
        SDL_SetRenderDrawColor(renderer, image->img_data[i], image->img_data[i + 1],
                               image->img_data[i + 2], 255);

        SDL_RenderDrawPoint(renderer, x, y);

        if (iter != 0 && iter % STRIP_WIDTH == 0) {
            x = STRIP_WIDTH * window_strip_number;
            y += 1;

            i = STRIP_WIDTH * img_strip_number * 3 + image->width * 3 * y;

            // printf("i: %d\n", i);

            continue;
        }

        x += 1;
        i += 3;
        // printf("i: %d\n", i);
    }
}

void render_image(SDL_Renderer *renderer, int array[], size_t array_size,
                  Image *image) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < array_size; i++) {
        paint_image_strip(renderer, image, i, array[i]);
    }

    SDL_Delay(ANIMATION_DELAY_MS);

    SDL_RenderPresent(renderer);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed! %s\n", SDL_GetError());
        return 1;
    }

    Image image = {};

    image.img_data = stbi_load("./testimage.jpg", &image.width, &image.height,
                               &image.channels, 3);

    printf("channels: %d, img_width: %d, img_height: %d\n", image.channels,
           image.width, image.height);

    SDL_Window *window = SDL_CreateWindow(
        "Mah window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        image.width, image.height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    int height, width;

    int quit = 0;
    SDL_Event e;

    int strip_number = 0;
    int sorting = 0;

    int array_size = image.width / STRIP_WIDTH;
    int array[array_size];

    for (int i = array_size - 1; i >= 0; i--) {
        array[i] = array_size - i - 1;
    }

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

                        case SDLK_s: {
                            printf("Starting sort\n");

                            sorting = 1;
                            binary_search(array, array_size, render_image,
                                          renderer, &image);
                            break;
                        }

                        default:
                            break;
                    }
                }
            }
        }

        if (!sorting) {
            render_image(renderer, array, array_size, &image);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
