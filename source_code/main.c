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

    image_point m_start;

	//we could eliminate m_dimensions from image_representation struct because it is already in m_texture, but for now we are just going to maintain it, just because i think that just having two floats
	//... is less costly than having to do a memory access from m_texture. In the current way though we are going to put more pressure in the registers of our processor, and it is also possible that there
	//is a lot of memory being used in one point of the program, having to mmove these values into memory rather than a register, for now we will just leave it like this.
    image_point m_dimensions;

    float m_angle;

    SDL_Texture* m_texture;


}image_representation;

/*
void calculateCenter(image_representation* image_file, SDL_FPoint* center_point) {


    if (image_file != NULL) {

        center_poin->y = image_file->m_dimensions.m_row / 2 + image_file->m_start.m_row;

        center_point->x = image_file->m_dimensions.m_column / 2 + image_file->m_start.m_column;

    }

return temporary;

}*/
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

void render_with_rotation(SDL_Renderer* renderer, image_representation* image_file){
//keep in mind that in image_file, we have already updated the angle of the image, which is stored inside of image_file already.
	SDL_FRect destinationRect = {0};

	destinationRect.x = image_file->m_start.m_column;

	destinationRect.y = image_file->m_start.m_row;

	destinationRect.h = image_file->m_dimensions.m_row;

	destinationRect.w = image_file->m_dimensions.m_column;

	//in SDL we need to use SDL_Fpoint
	SDL_FPoint temporaryPoint = {0};

	float center_m_row = image_file->m_dimensions.m_row / 2 + image_file->m_start.m_row;

        float center_m_column = image_file->m_dimensions.m_column / 2 + image_file->m_start.m_column;


	temporaryPoint.x = center_m_column - image_file->m_start.m_column;
	temporaryPoint.y = center_m_row - image_file->m_start.m_row;
	SDL_RenderTextureRotated(renderer, image_file->m_texture, NULL, &destinationRect, (double)image_file->m_angle, &temporaryPoint, SDL_FLIP_NONE);
}

void destroyImageRepresentation(image_representation* image_file) {

    if (image_file != NULL) {

        if (image_file->m_texture != NULL) {

            SDL_DestroyTexture(image_file->m_texture);

            image_file->m_texture = NULL;

        }

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

float calculateAngleQuadrent(float mouseX, float mouseY, float row_user, float column_user){

    float angle_final = 0.0f;

    float m_dif_x = (mouseX - column_user);

    float m_dif_y = (mouseY - row_user);

    float distance_between_points = sqrtf(m_dif_x * m_dif_x + m_dif_y * m_dif_y);

    angle_final = atan2f(m_dif_y,m_dif_x);

    angle_final = angle_final * (180.0f / M_PI);

    if(angle_final<0){

	angle_final+=360.0f;

    }

    return angle_final;

}


bool eventHandler(SDL_Event* event,float* mouseX, float* mouseY, image_representation* default_player){
	bool quit = false;
	bool movement = false;
        if (event->type == SDL_EVENT_QUIT) {


	return true;
            }
        if (event->type == SDL_EVENT_MOUSE_MOTION) {
			*(mouseX) = event->motion.x;
			*(mouseY) = event->motion.y;
			movement = true;
                }else if (event->type == SDL_EVENT_KEY_DOWN) {

			switch(event->key.key){

				case SDLK_UP:

					default_player->m_start.m_row -=2;

					movement = true;

					break;

				case SDLK_DOWN:

					default_player->m_start.m_row +=2;

					movement = true;

					break;
				case SDLK_LEFT:

					default_player->m_start.m_column -=2;

					movement = true;

					break;

				case SDLK_RIGHT:
					default_player->m_start.m_column +=2;

					movement = true;

					break;
				default:

					SDL_Log("Error, you have pressed a button that is not yet allowed");

					break;
			}

                    }

	if(movement){

		float center_m_row = (default_player->m_dimensions.m_row/2.0f) + default_player->m_start.m_row;

		float center_m_column = (default_player->m_dimensions.m_column/2.0f) + default_player->m_start.m_column;

	        default_player->m_angle = calculateAngleQuadrent(*(mouseX), *(mouseY), center_m_row,center_m_column);

	}


	return quit;

}


void rendering(SDL_Renderer* renderer, image_representation* default_player){

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderClear(renderer);

        render_with_rotation(renderer, default_player);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	float center_m_row = default_player->m_dimensions.m_row/2 + default_player->m_start.m_row;

	float center_m_column = default_player->m_dimensions.m_column/2 + default_player->m_start.m_column;

	SDL_RenderPoint(renderer, center_m_column, center_m_row);

        SDL_RenderPresent(renderer);


}

int main(int argc, char* argv[]) {

    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

	/*the following image_representation is just for the defaut, without any animations*/
    image_representation default_player = { 0 };
	/*the following we have two arrays of image_representations for two different types of animations*/
	image_representation player_punch_left[8];
	image_representation player_punch_right[8];

    int exit_code = 0;
/*This is the default image, just the normal image*/
    const char* image_path = "../videoGameImatges/default.png";
/*the following arrays of char strings are for animtaions, speicifly for punching left and punching right*/
    const char* image_path_punch_left[] = {"../videoGameImatges/punch_left/first.png","../videoGameImatges/punch_left/second.png",
    "../videoGameImatges/punch_left/third.png","../videoGameImatges/punch_left/fourth.png","../videoGameImatges/punch_left/fith.png",
    "../videoGameImatges/punch_left/sixth.png","../videoGameImatges/punch_left/seventh.png","../videoGameImatges/punch_left/vuit.png"
    };

    const char* image_path_punch_right[] = {"../videoGameImatges/punch_right/first.png","../videoGameImatges/punch_right/second.png",
    "../videoGameImatges/punch_right/third.png","../videoGameImatges/punch_right/fourth.png","../videoGameImatges/punch_right/fith.png",
    "../videoGameImatges/punch_right/sixth.png","../videoGameImatges/punch_right/seventh.png","../videoGameImatges/punch_right/vuit.png"
    };
    exit_code = init(&window, &renderer);

    if (exit_code != 0) {

        SDL_Log("error");

        return exit_code;

    }
//////////above this line in the main, we dont need to do anything to optmitze our code, or atleast it won't affect the performance as mush if we focus on what is down below
///i understand that i should focus on the main loop, but whatever
    exit_code = loadMedia(&default_player, image_path, renderer, 0.0f, 0.0f);
/*first punch left animation*/
	for (int i =0;i<8;i++){

	loadMedia(&player_punch_left[i], image_path_punch_left[i], renderer, 0.0f, 0.0f);
	}
	for (int i =0;i<8;i++){

	loadMedia(&player_punch_right[i], image_path_punch_right[i], renderer, 0.0f, 0.0f);

	}


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

        while (SDL_PollEvent(&event)){

		quit = eventHandler(&event, &mouseX, &mouseY, &default_player);
        }

		rendering(renderer, &default_player);

    }


        destroyImageRepresentation(&default_player);

        closeRendererWindow(&renderer, &window);

        return 0;
}
