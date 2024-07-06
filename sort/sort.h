#ifndef SORT
#define SORT

#include <SDL2/SDL_render.h>
#include <sys/types.h>
#include "../stb_image.h"

typedef struct {
    stbi_uc *img_data;
    int width;
    int height;
    int channels;
} Image;

typedef void (*callback)(SDL_Renderer *renderer, int array[], size_t array_size,
                         Image *image);

void binary_sort(int array[], size_t length, callback callback_func, SDL_Renderer *renderer, Image *image);
void shuffle_array(int array[], size_t length);

#endif // SORT
