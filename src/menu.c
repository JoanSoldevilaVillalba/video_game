#include "menu.h"

//function that presents options for user in main program loop

void printMenu(){

	printf("Options: \n");

	printf("1. Connect/communicate to server\n");

	printf("2. Enter game\n");

	printf("3. Characters\n");

	printf("4. Quit game/program\n");

	printf("Enter a number please: \n");

}

int menuCallFunction(){
	int result = 0;

	bool quit = false;


	printf("Welcome to th e main menu\n");

	do{

		printMenu();

		scanf("%d",&result);

		switch(result){

			case 1:
			case 2:

				quit = true;

				break;

			case 3:

				break;

			case 4:
				printf("Your are quitting the game, goodbye ...\n");

				break;
			default:

				printf("Error, you enterd a wrong number\n");

				break;

		}

	}while(!quit);

	return result;

}
