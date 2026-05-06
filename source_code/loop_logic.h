#pragma once
#include <stdbool.h>
#include <math.h>

#include "image.h"

void rendering(SDL_Renderer* renderer, image_representation* default_player);


float calculateAngleQuadrent(float mouseX, float mouseY, float row_user, float column_user);

bool eventHandler(SDL_Event* event,float* mouseX, float* mouseY, image_representation* default_player);

void handleGameLoop(SDL_Renderer* renderer, image_representation* default_player);
