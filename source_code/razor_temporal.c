#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <math.h>
const int screenWidth = 640;
const int screenHeight = 480;

typedef struct {


    float m_row;

    float m_column;


}image_point;

typedef struct {


    image_point m_center;

    image_point m_start;

    image_point m_dimensions;

    float m_angle;

    SDL_Texture* m_texture;


}image_representation;


void  calculateCenter(image_representation* image_file) {

    if (image_file != NULL) {

        image_file->m_center.m_row = image_file->m_dimensions.m_row / 2 + image_file->m_start.m_row;

        image_file->m_center.m_column = image_file->m_dimensions.m_column / 2 + image_file->m_start.m_column;

    }


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

            calculateCenter(image_file);

        }

    }


    SDL_DestroySurface(temporarySurface);

    return exit_code;

}



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


void render(SDL_Renderer* renderer, image_representation* image_file) {

    SDL_FRect destinationRect;

    destinationRect.x = image_file->m_start.m_column;

    destinationRect.y = image_file->m_start.m_row;

    destinationRect.h = image_file->m_dimensions.m_row;

    destinationRect.w = image_file->m_dimensions.m_column;

    SDL_RenderTexture(renderer, image_file->m_texture, NULL, &destinationRect);

}
/*
void render(SDL_Renderer* renderer, image_representation* image_file){

        SDL_FRect destinationRect;

        destinationRect.x = image_file->m_start.m_column;

        destinationRect.y = image_file->m_start.m_row;

        destinationRect.h = image_file->m_dimensions.m_row;

        destinationRect.w = image_file->m_dimensions.m_column;

        SDL_RenderTextureRotated(renderer, image_file->m_texture,NULL, &destinationRect, degrees, image_file->m_center);



}*/
void destroyImageRepresentation(image_representation* image_file) {

    if (image_file != NULL) {

        if (image_file->m_texture != NULL) {

            SDL_DestroyTexture(image_file->m_texture);

            image_file->m_texture = NULL;

        }


        image_file->m_center.m_row = 0.0f;

        image_file->m_center.m_column = 0.0f;

        image_file->m_start.m_row = 0.0f;

        image_file->m_start.m_row = 0.0f;

        image_file->m_dimensions.m_row = 0.0f;

        image_file->m_dimensions.m_row = 0.0f;

        image_file->m_angle = 0.0f;

    }


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

float calculateAngleQuadrent(float mouseX, float mouseY, float row_user, float column_user) {

    float angle_final = 0.0f;

    float m_dif_x = (mouseX - column_user);

    float m_dif_y = (mouseY - row_user);

    printf("diferencia de x (primer mouse i despres user): %f - %f = %f\n", mouseX, row_user, m_dif_x);

    printf("diferencia de y (primer mouse i despres user): %f - %f = %f\n", mouseY, column_user, m_dif_y);


    float distance_between_points = sqrtf(m_dif_x * m_dif_x + m_dif_y * m_dif_y);
printf("distance between points: %f\n", distance_between_points);


	angle_final = atan2f(m_dif_y,m_dif_x);
	angle_final = angle_final * (180.0f / M_PI);
	if(angle_final<0){

		angle_final+=360.0f;

	}

	printf("angle final retornat: %f\n\n\n", angle_final);
    return angle_final;

}


int main(int argc, char* argv[]) {

    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    image_representation default_player = { 0 };

    int exit_code = 0;

    const char* image_path = "../videoGameImatges/default.png";

    exit_code = init(&window, &renderer);

    if (exit_code != 0) {

        SDL_Log("error");

        return exit_code;

    }

    exit_code = loadMedia(&default_player, image_path, renderer, 0.0f, 0.0f);

    if (exit_code != 0) {

        SDL_Log("error");

        return exit_code;

    }


    bool quit = false;

    SDL_Event event;

    SDL_zero(event);

    image_representation* current_image = NULL;

    int counter_image = 0;

    float mouseX = 0.0f;

    float mouseY = 0.0f;


    while (!quit) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_EVENT_QUIT) {

                quit = true;

            }
            else {

                if (event.type == SDL_EVENT_MOUSE_MOTION) {

                    uint32_t buttons = SDL_GetMouseState(&mouseX, &mouseY);

                }

                if (event.type == SDL_EVENT_KEY_DOWN) {
                    //every time that we update the images staring rendering positino, we are also going to have to update its center

                    if (event.key.key == SDLK_UP) {

                        default_player.m_start.m_row -= 2;

                        default_player.m_center.m_row -= 2;

                    }
                    if (event.key.key == SDLK_DOWN) {

                        default_player.m_start.m_row += 2;

                        default_player.m_center.m_row += 2;

                    }
                    if (event.key.key == SDLK_LEFT) {

                        default_player.m_start.m_column -= 2;

                        default_player.m_center.m_column -= 2;

                    }
                    if (event.key.key == SDLK_RIGHT) {

                        default_player.m_start.m_column += 2;

                        default_player.m_center.m_column += 2;

                    }


                }


                default_player.m_angle = calculateAngleQuadrent(mouseX, mouseY, default_player.m_center.m_row, default_player.m_center.m_column);


            }


SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw your player image
        render(renderer, &default_player);

        // Draw a RED pixel at the player's center point
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderPoint(renderer, default_player.m_center.m_column, default_player.m_center.m_row);

        // Show the result
        SDL_RenderPresent(renderer);



        }

    }
        destroyImageRepresentation(&default_player);

        closeRendererWindow(&renderer, &window);

        return 0;
    }
