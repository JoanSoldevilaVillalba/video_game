#include "initilize.h"

#include "loop_logic.h"

int main(int argc, char* argv[]) {

    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    image_representation default_player = { 0 };

    image_representation player_punch_left[8];

    image_representation player_punch_right[8];

    int exit_code = 0;

    const char* image_path = "../videoGameImatges/default.png";

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

    exit_code = loadMedia(&default_player, image_path, renderer, 0.0f, 0.0f);

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
