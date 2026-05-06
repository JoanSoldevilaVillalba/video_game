#include "menu.h"

void printMenu(){
	printf("Options: \n");
	printf("1. Connect/communicate to server\n");
	printf("2. Enter game\n");
	printf("3. Characters\n");
	printf("4. Quit game/program\n");
	printf("Enter a number please: \n");
}

/*menuCallFunction is called in main, this function is called when the game is loaded, so that the usser is able to see a menu. Keep in mind that in the future the menu will be done using sdl aswell, but for now we are going to base it in the terminal*/
/*for now we are just going to assume that the user enterse a number each time, no error detection, later we are giong to do some error detection.*/
int menuCallFunction(){
	int result = 0;
	printf("Welcome to th e main menu\n");
	bool quit = false;
	do{

		printMenu();
		scanf("%d",&result);

		switch(result){
//both 1,2 for now require us to leave this fuynction, so we are goint to have to indicate ti some how:
		case 1:
		case 2:
			quit = true;
			break;
		case 3:
//we are  going to implement this later
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
