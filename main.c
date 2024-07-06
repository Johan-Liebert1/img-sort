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
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "sort/sort.h"

#define WIN_ASPECT_RATIO_FACTOR 100
#define STRIP_WIDTH 25
#define ANIMATION_DELAY_MS 0

enum SortType {
    BinarySort,
};

typedef struct {
    Image *image;
    SDL_Renderer *renderer;
    enum SortType sort_type;
    size_t array_size;
    int *array;
} StartSort;

// Unused for now
pthread_mutex_t renderer_mutex;

void paint_image_strip(SDL_Renderer *renderer, Image *image, int img_strip_number, int window_strip_number) {
    int strip_height = image->height;
    int num_channels = image->channels;

    // Create an SDL texture to hold the image strip
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, STRIP_WIDTH, strip_height);

    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        return;
    }

    Uint32 *pixels = (Uint32 *)malloc(STRIP_WIDTH * strip_height * sizeof(Uint32));

    if (!pixels) {
        fprintf(stderr, "Buy more ram lol\n");
        SDL_DestroyTexture(texture);
        return;
    }

    for (int y = 0; y < strip_height; y++) {
        for (int x = 0; x < STRIP_WIDTH; x++) {
            int img_index = (y * image->width + (img_strip_number * STRIP_WIDTH + x)) * num_channels;

            int pix_index = y * STRIP_WIDTH + x;

            Uint8 r = image->img_data[img_index];
            Uint8 g = image->img_data[img_index + 1];
            Uint8 b = image->img_data[img_index + 2];
            Uint8 a = 255;

            pixels[pix_index] = (r << 24) | (g << 16) | (b << 8) | a;
        }
    }

    // Update the texture with the pixel data
    SDL_UpdateTexture(texture, NULL, pixels, STRIP_WIDTH * sizeof(Uint32));
    SDL_Rect dest_rect = {STRIP_WIDTH * window_strip_number, 0, STRIP_WIDTH, strip_height};

    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    free(pixels);
    SDL_DestroyTexture(texture);
}

void paint_image_strip_slow_af(SDL_Renderer *renderer, Image *image, int img_strip_number, int window_strip_number) {
    int num_iterations = STRIP_WIDTH * image->height;

    int i = STRIP_WIDTH * img_strip_number * image->channels;

    int x = STRIP_WIDTH * window_strip_number;
    int y = 0;

    for (int iter = 0; iter <= num_iterations; iter++) {
        SDL_SetRenderDrawColor(renderer, image->img_data[i], image->img_data[i + 1], image->img_data[i + 2], 255);

        SDL_RenderDrawPoint(renderer, x, y);

        if (iter != 0 && iter % STRIP_WIDTH == 0) {
            x = STRIP_WIDTH * window_strip_number;
            y += 1;

            i = STRIP_WIDTH * img_strip_number * image->channels + image->width * image->channels * y;

            continue;
        }

        x += 1;
        i += image->channels;
    }
}

void render_image(SDL_Renderer *renderer, int array[], size_t array_size, Image *image) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < array_size; i++) {
        paint_image_strip(renderer, image, i, array[i]);
    }

    if (ANIMATION_DELAY_MS > 0) {
        SDL_Delay(ANIMATION_DELAY_MS);
    }

    SDL_RenderPresent(renderer);
}

void *start_sort(void *arg) {
    StartSort *start_sort = (StartSort *)arg;

    printf("renderer: %p\n", start_sort->renderer);

    switch (start_sort->sort_type) {
        case BinarySort:
            binary_sort(start_sort->array, start_sort->array_size, render_image, start_sort->renderer, start_sort->image);
            break;
    }

    return NULL;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed! %s\n", SDL_GetError());
        return 1;
    }

    Image image = {};

    image.img_data = stbi_load("./testimage.jpg", &image.width, &image.height, &image.channels, 3);

    printf("channels: %d, img_width: %d, img_height: %d\n", image.channels, image.width, image.height);

    SDL_Window *window = SDL_CreateWindow("Mah window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image.width, image.height,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int height, width;

    int quit = 0;
    SDL_Event e;

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

                            pthread_t thread_id;

                            StartSort args = {
                                .image = &image,
                                .renderer = renderer,
                                .sort_type = BinarySort,
                                .array = array,
                                .array_size = array_size,
                            };

                            start_sort(&args);

                            // Apparantly it's illegal to render in a separate thread.
                            // Mutex locking doesn't work either
                            //
                            // pthread_create(&thread_id, NULL, start_sort, &args);

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
