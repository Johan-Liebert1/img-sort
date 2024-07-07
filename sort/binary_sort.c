#include "sort.h"

void shuffle_array(int *array, size_t length) {
}

void binary_sort(int *array, size_t length, callback callback_func, SDL_Renderer *renderer, Image *image, FFMpeg *ffmpeg) {
    for (int i = 0; i < length; i++) {
        for (int j = i; j < length; j++) {
            if (array[i] > array[j]) {
                array[j] = array[i] ^ array[j];
                array[i] = array[i] ^ array[j];
                array[j] = array[i] ^ array[j];

                callback_func(renderer, array, length, image, ffmpeg);
            }
        }
    }
}
