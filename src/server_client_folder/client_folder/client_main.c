#include "client_main.h"
#include "client_send_data.h"

typedef enum{

//the following are options when the user wants to play the game and either has to: wait for another player to enter, needs to close the connection because nobody wants to play or games are full and when a game has been found and the client side has to switch context

	GAME_FOUND = 1,

	WAITING_FOR_GAME = 2,

	GAME_NOT_FOUND = 3,

}SECOND_LAYER_WAITING;



typedef enum{

//the following are options for quitting. There are differnt types of quitting: when the connection is broken, when the server decided to quit, when the client decided to quit (server deciding to quit is redundant because the client always initiates, but we are still going to add it)

	CLIENT_QUIT = 4,

	SERVER_QUIT = 5,

	BROKEN_QUIT = 6


}SECOND_LAYER_QUIT;




typedef enum{ 
//this typedef enumerator is used for when the server has found a game, and now the client is able to start playing the game

	MENU = 7,

	PLAY_TIME = 8,

	PROLOGUE = 9


}SECOND_LAYER_PLAY;




typedef enum{

	ENTERING_CREATING_GAME = 0,

	QUIT = 1,

	RANDOM_MESSAGE = 2,

	GAME_PLAY = 3,


}FIRST_LAYER;

int menu_fd_parser(char* pointer_string, int* counter, char* end_terminator){


// inside of pointer_string, there is going to be something like the following message: 3|7|132|21321
//first two numbers are just for protocol, last to integers are the actual integers that we want to receive in order to display this in menu
//we use memset in every iteratrion of the while loop, meaning that at the end of the stirng, we know there is a null terminator
	int i = *(counter);

	int current = 0;

	int temporary = 0;

	while(*(pointer_string + i) !=*(end_terminator)){

		temporary = *(pointer_string + i) - '0';

		current = current * 10;

		current = current + temporary;

		i++;

	}

	*(counter) = ++i; //i always points to the correct next character

	return current;

}



int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port){

	*(client_file_descriptor) = socket(AF_INET, SOCK_STREAM,0);

	if(*(client_file_descriptor)<0){

		printf("Error, no file descriptor was assigned to client: %s\n",strerror( errno));

		return -1;

	}


	server_address->sin_family = AF_INET;

	server_address->sin_port = htons(port);


	int result_translation = 0;

	int status = 0;


	result_translation =  inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr);

	if(result_translation <=0){

		printf("Error, possible incorrect format: %s\n", strerror(errno));

		return -1;

	}


	status = connect(*(client_file_descriptor), (struct sockaddr*)server_address, sizeof(*(server_address)));

	if(status<0){

		printf("Error, connection with server was not possible: %s\n",strerror(errno));

		return -1;

	}


	return 0;

}


void printMenuSC(){

	printf("---- CLIENT MENU ----\n");
	printf("0. PLAY GAME\n");
	printf("1. QUIT GAME\n");

}

//before sending information, we need to first validate that the length of the message is not bigger than BUFFER_SIZE, we need to validate this.

bool validate_message_length(const char* temporary_pointer){

        if(BUFFER_SIZE <= strlen(temporary_pointer)){

                return false;

        }else{

                return true;

        }

        //rememeber that strlen is indexed to one,

}



ssize_t send_validated_message(const char* temporary_pointer, char buffer[BUFFER_SIZE], int client_file_descriptor){

	bool correct_length = validate_message_length(temporary_pointer);

	ssize_t result = 0;

	if(correct_length == true){

		strncpy(buffer, temporary_pointer, strlen(temporary_pointer));

		buffer[strlen(temporary_pointer)] = '\0';

		result = send_framed_message(client_file_descriptor, buffer, (uint32_t)strlen(temporary_pointer));

		if(result == -1){

			printf("Unable to send message to server: %s\n", strerror(errno));

			result = -1;

		}

	}else{

		printf("Error, the following message exceeds the BUFFER_SIZE limit: %s\n", temporary_pointer);

		result = -2;

	}



	return result;

}


int handleServerCommunication(int server_port){

	char buffer_send[BUFFER_SIZE]={0}, buffer_receive[BUFFER_SIZE]={0}; const char* temporary_buffer =NULL;

	struct sockaddr_in server_address;

	ssize_t result_send_receive = 0;

	int option = 0, client_file_descriptor = -1, first_number = -1, second_number = -1, counter = 0, client_1 = -1, client_2 = -1;

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

				memory = true; //this does not really matter

			}else if(result_send_receive == -2){

				//when string exceeds buffer limit that we have defined (BUFFER_SIZE)

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

						//the server can only respond with two possible messages if the server is trying to find a gamne for us:

						//0|3: game not found, time expired

						//0|1: game was found

						/*
						For the purpose of testing on how our client-server model behaves we are going to do the folloiwng:

						if the response is gmae was found, we are going just simply quit

						if a game was found, we are just going to go to the pro

						*/

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

							//inside buffer_receive, we have both names that the server has assigned to use: remember that for now the names are the server side file descriptors of both players that enterd the same game

							//we are going to supose that both file descriptors are separated by the following charcter: '|'

							counter = 4;

							//current implementation wil not work if there are multiple numberss for each file descriptor/client that is  connected to the game

							char* null_terminator_temp = "|";

							client_1 =  menu_fd_parser(buffer_receive, &counter, null_terminator_temp);

							null_terminator_temp=="\0";

							client_2 = menu_fd_parser(buffer_receive + counter, &counter,null_terminator_temp );

							printf("----- GAME MENU -----");

							printf("---------------------");

							printf("First player: %d\n", client_1);

							printf("Second pllayer: %d\n", client_2);

							//we are going to have to continue the development, stopping for now


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
