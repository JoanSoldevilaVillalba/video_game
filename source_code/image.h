#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct {


    float m_row;

    float m_column;


}image_point;

typedef struct {

    image_point m_start;

        //we could eliminate m_dimensions from image_representation struct because it is already in m_texture, but for now we are just going to maintain it, just because i think that just having two floats
        //... is less costly than having to do a memory access from m_texture. In the current way though we are going to put more pressure in the registers of our processor, and it is also possible that there
        //is a lot of memory being used in one point of the program, having to mmove these values into memory rather than a register, for now we will just leave it like this.
    image_point m_dimensions;

    float m_angle;

    SDL_Texture* m_texture;


}image_representation;


void render(SDL_Renderer* renderer, image_representation* image_file);

void render_with_rotation(SDL_Renderer* renderer, image_representation* image_file);

void destroyImageRepresentation(image_representation* image_file);
