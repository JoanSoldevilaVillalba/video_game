#pragma once


#include "image.h"

#include <SDL3_image/SDL_image.h>

#define screenWidth  680 
#define screenHeight  500


int init(SDL_Window** window, SDL_Renderer** renderer);

int loadMedia(image_representation* image_file, const char* image_path, SDL_Renderer* renderer, float startingX, float startingY);

int loadFromFile(const char* image_path, image_representation* image_file, SDL_Renderer* renderer, float startingX, float startingY);

void closeRendererWindow(SDL_Renderer** renderer, SDL_Window** window);
