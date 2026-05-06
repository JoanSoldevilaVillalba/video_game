#include "initilize.h"
#include "menu.h"
#include "loop_logic.h"
#include "server_folder/client_create_game.h"

int main(int argc, char* argv[]){

    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    image_representation default_player = { 0 };

    image_representation player_punch_left[8];

    image_representation player_punch_right[8];

    image_representation* current_image = NULL;

    int exit_code = 0;

    exit_code = init(&window, &renderer);

	bool quit = false;

    const char* image_path = "../videoGameImatges/default.png";

    const char* image_path_punch_left[] = {"../videoGameImatges/punch_left/first.png","../videoGameImatges/punch_left/second.png",
    "../videoGameImatges/punch_left/third.png","../videoGameImatges/punch_left/fourth.png","../videoGameImatges/punch_left/fith.png",
    "../videoGameImatges/punch_left/sixth.png","../videoGameImatges/punch_left/seventh.png","../videoGameImatges/punch_left/vuit.png"
    };

    const char* image_path_punch_right[] = {"../videoGameImatges/punch_right/first.png","../videoGameImatges/punch_right/second.png",
    "../videoGameImatges/punch_right/third.png","../videoGameImatges/punch_right/fourth.png","../videoGameImatges/punch_right/fith.png",
    "../videoGameImatges/punch_right/sixth.png","../videoGameImatges/punch_right/seventh.png","../videoGameImatges/punch_right/vuit.png"
    };

	//in the following lines of code  we are preparing all images that are going to be used in game
    exit_code = loadMedia(&default_player, image_path, renderer, 0.0f, 0.0f);

    for (int i =0;i<8;i++){

	loadMedia(&player_punch_left[i], image_path_punch_left[i], renderer, 0.0f, 0.0f);

    }

    for (int i =0;i<8;i++){

	loadMedia(&player_punch_right[i], image_path_punch_right[i], renderer, 0.0f, 0.0f);

    }
while(!quit){

    int option = menuCallFunction();
    switch(option){

	case 1:
		handleServerCommunication(8080);
		break;

	case 2:
		handleGameLoop(renderer, &default_player);
		break;

	case 3:
	//	showCharactersMenu();
		break;

	case 4:
		quit = true;
		break;


	default:
		printf("invalid\n");
		break;

	}



}
        destroyImageRepresentation(&default_player);

        closeRendererWindow(&renderer, &window);

        return 0;
}
