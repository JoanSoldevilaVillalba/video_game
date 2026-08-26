#include "client_main.h"
#include "client_send_data.h"

int handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0}, buffer_receive[BUFFER_SIZE]={0}; const char* temporary_buffer =NULL;

	struct sockaddr_in server_address;

	ssize_t result_send_receive = 0;

	int option = 0, client_file_descriptor = -1, first_number = -1, second_number = -1, counter = 0, client_1 = -1, client_2 = -1;

	int time_experation = 0;

        bool quit = false, memory = false, input = true;


	memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive,0,sizeof(buffer_receive)); memset(&server_address,0,sizeof(server_address));


	if(setupConnection(&client_file_descriptor, &server_address, server_port) == -1){

		printf("Error, unable to establish connection with the server\nExiting...");

		return -1;

	}

	printf("\n----- Client Interface -----\n");

	while(!quit){

		temporary_buffer = NULL;

		counter = 0;

		if(input == true){

			printMenuSC();

			printf("Enter a number down below:\n");

			do{

				scanf("%d", &option);

				if(option<0 || option>3){

					printf("Error, the number that you have enterd is invalid. Try again (look at the menu).\n");

				}

			}while(option<0 || option>3);

			memory = false;

		}

		if(memory == false){

			switch(option){

				case ENTERING_CREATING_GAME:

					temporary_buffer = "0|0|client wants game";

					memory = true;

					first_number = ENTERING_CREATING_GAME;

					break;

				case QUIT:

					temporary_buffer ="1|0|client wants to quit";

					memory = false;

					quit = true;

					first_number = QUIT;

					second_number = CLIENT_QUIT;

					break;;

				case RANDOM_MESSAGE:

					temporary_buffer = "2|0|This is a random message";

					break;


			}

			result_send_receive = send_validated_message(temporary_buffer, buffer_send,client_file_descriptor);

			if(result_send_receive == -1){

				first_number = QUIT;

				second_number = BROKEN_QUIT;

				quit = true;

				memory = true;

			}else if(result_send_receive == -2){

				memory = false;

				continue;

			}

			if(second_number !=BROKEN_QUIT){

	                        result_send_receive = receive_framed_message(client_file_descriptor, buffer_receive, (ssize_t)BUFFER_SIZE);

				if(result_send_receive == -1){

					second_number = BROKEN_QUIT;

					quit = true;

				}else{

		                        first_number = option; //first option is ENTERING_CREATING_GAME, PLAY_GAME or QUIT

                		        counter = 0;

		                        counter = counter + 2;

		                        second_number = buffer_receive[counter] - '0'; //conveting char to int for second_number

				}

			}

		}


		temporary_buffer = NULL;

		switch(first_number){


			case ENTERING_CREATING_GAME:

				switch(second_number){

					case WAITING_FOR_GAME:


						printf("We are waiting for a second player to enter our game\n");

						printf("Waiting for servers response ...\n");

						result_send_receive = receive_framed_message(client_file_descriptor, buffer_receive, (ssize_t)BUFFER_SIZE);

						if(result_send_receive == -1){

		                                        second_number = BROKEN_QUIT;

							first_number = QUIT;

							memory = true;

							input = false;

							break;

		                                }

						printf("Server responded with the following message: %s\n", buffer_receive);

						memory = true;

						first_number = ENTERING_CREATING_GAME;

						counter = 2;

						second_number = buffer_receive[counter]-'0';

						break;

					case GAME_NOT_FOUND :

						printf("The server was not able to find a game for us, server response: %s\n", buffer_receive);

						//we are just going to let the client decide what he or she wants to do once a game is not found

						counter = 0;

						first_number = QUIT;

						second_number = CLIENT_QUIT;

						memory = false; //here we do want to send a quit statment to the server

						input = false;

						break;

					case GAME_FOUND:

						//when a game is found, within the server side, the server saves the index of the game that we are playing 

						printf("Game was found\n");

						first_number = GAME_PLAY;

						second_number = PROLOGUE;

						memory = true;

						input = false;

						break;

					case STILL_WAITING:

						printf("After showing menu information, the other player quit. Still waiting for someone to join our game\n");

						printf("Do you still want to wait for someone to enter your game(y/n)?\n");

						char waiting_choice;

						do{
							printf("Please enter you final decision down below:\n");

							scanf("%c", &waiting_choice);

							if(waiting_choice !='y' || waiting_choice !='n'){

								printf("Error, invalid input\n Enter y for yes or n for no: \n");

							}

						}while(waiting_choice !='y' || waiting_choice !='n');


						if(waiting_choice == 'y'){

							printf("How long do you want to wait for?");

							do{

								printf("Enter a number down below (seconds):\n");

								scanf("%d", &time_experation);

								if(time_experation<=0 || time_experation >=360){

									printf("Error: enter a number between <120> and <360>\n");

								}

							}while(time_experation<=0 || time_experation >=360);

						temporary_buffer=""; //here besides the protocol numbers we are going to have to add the time_experation, 
						}else{

							//in this case, we just need to indicate to the server that we do not want to quit, but rather just empty our slot in game_list global variable to free up our slot for someone else to use

							temporary_buffer="";

							input = false;

							memory = false;

						}
						break;


				}


				break;

			case GAME_PLAY:

					switch(second_number){

						case PROLOGUE:

							printf("Client is now waiting for the menu ....");

							temporary_buffer = "3|0|client waiting for menu";

							first_number = GAME_PLAY;

							second_number = MENU;

							memory = false;

							input = false;


							break;

						case MENU:

							counter = 4;

							client_1 =  menu_fd_parser(buffer_receive, &counter, '|');

							client_2 = menu_fd_parser(buffer_receive + counter, &counter,'\0');

							printf("----- GAME MENU -----");

							printf("---------------------");

							printf("First player: %d\n", client_1);

							printf("Second pllayer: %d\n", client_2);

							printf("Do you want to play (1) or quit (0)?");
							printf("Enter your option down below\n");

							do{

								scanf("%d", &option);

								if(option<0 || option>1){

									printf("Error, invalid input, must enter zero or one\n");

								}

							}while(option<0 || option>1);


							memset(buffer_receive,0,sizeof(buffer_receive));

							buffer_receive = "4|i|";

							buffer_receive[4] = option + '0';

							temporary_buffer = buffer_receive;

							memset(buffer_receive, 0, sizeof(buffer_receive));

							memory = false;
							input = false;

							break;


						case PLAY_TIME:



							break;

					}

				break;


			case QUIT:

					switch(second_number){


						case CLIENT_QUIT:

							printf("Client wants to quit. Servers response: %s. Client is closing connection. Goodbye ...\n", buffer_receive);

							break;


						case SERVER_QUIT:

							printf("Server wants to quit for the following reason:; %s\n", buffer_receive);

							printf("Closing connection. Goodbye ...\n");

							break;



						case BROKEN_QUIT:

							printf("Broken connection: unable to receive or send information to server. Closing connection. Goodbye ...\n");

							break;

						case OTHER_PLAYER_QUIT:

							printf("The other player has decided to quit unforunatly. We are exeting the game. Closing connection\n");

							break;

						case YOU_PLAYER_QUIT:

							printf("You have decided to quit. Other player has received the new. Closing the connection");

							break;

					}

					quit = true;

				break;

			}


		}


	close(client_file_descriptor);


	return 0;

}

int main(){

	int port = 8080;

	int result_communication = handleServerCommunication(port);

	if(result_communication == -1){

		printf("Error, closing client now\n");

	}else{

		printf("Closing client with no problems\n");

	}

}
