#include "initilize.h"
#include "menu.h"
#include "loop_logic.h"
#include "server_folder/client_create_game.h"

#define n_images_animation 8 //8 is the total amount of images that are used for punching animation.

#define server_port 8080 //this is the port that the server is going to be runing in my computer

int main(int argc, char* argv[]){

	SDL_Window* window = NULL;

	SDL_Renderer* renderer = NULL;

	image_representation default_player = { 0 };

	image_representation player_punch_left[n_images_animation];

	image_representation player_punch_right[n_images_animation];

	image_representation* current_image = NULL;

	int exit_code = 0;

	bool quit = false;

	int option = 0;

	//this is the string path for the default character that the user can use to play the game

	const char* image_path = "../videoGameImatges/default.png";

	//here we have two sets  of string arrays

	const char* image_path_punch_left[] = {"../videoGameImatges/punch_left/first.png","../videoGameImatges/punch_left/second.png",
	"../videoGameImatges/punch_left/third.png","../videoGameImatges/punch_left/fourth.png","../videoGameImatges/punch_left/fith.png",
	"../videoGameImatges/punch_left/sixth.png","../videoGameImatges/punch_left/seventh.png","../videoGameImatges/punch_left/vuit.png"};

	const char* image_path_punch_right[] = {"../videoGameImatges/punch_right/first.png","../videoGameImatges/punch_right/second.png",
	"../videoGameImatges/punch_right/third.png","../videoGameImatges/punch_right/fourth.png","../videoGameImatges/punch_right/fith.png",
	"../videoGameImatges/punch_right/sixth.png","../videoGameImatges/punch_right/seventh.png","../videoGameImatges/punch_right/vuit.png"};


	//this function initilizes the window and the renderer, changing what position in memory they point to

	exit_code = init(&window, &renderer);

	//allocating memory for the default image of the player

	exit_code = loadMedia(&default_player, image_path, renderer, 0.0f, 0.0f);

	//allocating memory for the animation images when the player punches left and right

	for (int i =0;i<n_images_animation;i++){

		loadMedia(&player_punch_left[i], image_path_punch_left[i], renderer, 0.0f, 0.0f);

	}

	for (int i =0;i<n_images_animation;i++){

		loadMedia(&player_punch_right[i], image_path_punch_right[i], renderer, 0.0f, 0.0f);

	}

	//this is the main loop of the hole video game/program, but it is not the game loop
	//here we have several options: we can ask the menu to communicate with the server
	//we can go into a game
	//we can see all of the characters that we have
	//...
	//keep in mind that this is a very premative state of the final product
	while(!quit){

		option =  menuCallFunction();

		switch(option){

			case 1:

				handleServerCommunication(server_port);

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

	//here we are dealocating and destroying the images of the video game aswell as the window and the renderer.

	destroyImageRepresentation(&default_player);

        closeRendererWindow(&renderer, &window);

        return 0;
}
