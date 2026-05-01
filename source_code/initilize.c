#include "initilize.h"
int init(SDL_Window** window, SDL_Renderer** renderer) {

    int exit_code = 0;

    if (SDL_Init(SDL_INIT_VIDEO)) {

        if (!SDL_CreateWindowAndRenderer("SDL3 tutorial: this is just for practice", screenWidth, screenHeight, 0, window, renderer)) {
            exit_code = 1;
        }

    }
    else {

        exit_code = 1;
    }

    return exit_code;

}

int loadMedia(image_representation* image_file, const char* image_path, SDL_Renderer* renderer, float startingX, float startingY) {

    int exit_code = 0;

    exit_code = loadFromFile(image_path, image_file, renderer, startingX, startingY);

    return exit_code;

}

int loadFromFile(const char* image_path, image_representation* image_file, SDL_Renderer* renderer, float startingX, float startingY) {

    int exit_code = 0;

    SDL_Surface* temporarySurface = IMG_Load(image_path);

    if (temporarySurface == NULL) {

        SDL_Log("Error, we were not able to load a surface %s\n", SDL_GetError());

        exit_code = 1;

    }
    else {

        image_file->m_texture = SDL_CreateTextureFromSurface(renderer, temporarySurface);

        if (image_file->m_texture == NULL) {

            SDL_Log("Error, we were not able to create a texture from the surface");

            exit_code = 1;
        }
        else {

            image_file->m_start.m_row = startingX;

            image_file->m_start.m_column = startingY;

            image_file->m_dimensions.m_row = temporarySurface->h;

            image_file->m_dimensions.m_column = temporarySurface->w;

        }

    }


    SDL_DestroySurface(temporarySurface);

    return exit_code;

}

void closeRendererWindow(SDL_Renderer** renderer, SDL_Window** window) {

    if (*(renderer) != NULL) {

        SDL_DestroyRenderer(*(renderer));

        *(renderer) = NULL;

    }

    if (*(window) != NULL) {

        SDL_DestroyWindow(*(window));

        *(window) = NULL;

    }

}
