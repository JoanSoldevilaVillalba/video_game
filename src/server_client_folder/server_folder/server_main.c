#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "server_send_data.h"

#include "server_logic.h"

#define BUFFER_SIZE 64

#define MAX_GAMES_SIZE 2

#define MAX_CLIENT_THREADS (MAX_GAMES_SIZE*2)


typedef enum{

ENTERING_CREATING_GAME = 0,

QUIT = 1,

RANDOM_MESSAGE = 2,

MENU_PREPERATION = 3

}FIRST_LAYER;


typedef struct{

int first_player;

int second_player;

char name[62];

int game_id;

pthread_cond_t game_condition;

}game_struct_players;



pthread_mutex_t mutex_game_list;

pthread_mutex_t mutex_thread_counter;

pthread_cond_t conditional_variable;

int counter_thread;

typedef struct{

int socket_fd;

game_struct_players* pointer_list_game;

}struct_client;



void* handle_client(void* arg){

	struct_client* client = (struct_client*)arg;

	pthread_detach(pthread_self());

	char buffer_receive[BUFFER_SIZE], buffer_send [BUFFER_SIZE];

	int client_game_id = -1,result = 0, counter = 0, first_number = 0, second_number = 0, index_game = -1;

	bool quit = false;

	ssize_t bytes_result = 0;

	const char* temporary_pointer = NULL;

	while(!quit){

		memset(buffer_receive, 0, sizeof(buffer_receive)); memset(buffer_send, 0, sizeof(buffer_send)); temporary_pointer = NULL; counter = 0 ;

		//we need a way to check what  we are receving does not exceed the limit of BUFFER_SIZE

		bytes_result = receive_validated_message(buffer_receive, client->socket_fd);

		if(bytes_result == -1){

			printf("Error, closing the connection with client, %s\n", strerror(errno));

			quit = true;

			break; //we are simply breaking because the connection is already broken, no need for more main while loop code

		}

		printf("We have received the following message from the client: %s\n", buffer_receive);

		counter = 0;

		first_number = buffer_receive[counter] - '0';

		switch(first_number){


			case ENTERING_CREATING_GAME:

				struct timespec ts;

				clock_gettime(CLOCK_REALTIME, &ts);

				ts.tv_sec +=5;

				int timed_out = 0;

				memset(buffer_send, 0, sizeof(buffer_send));

				temporary_pointer = create_game(client->socket_fd, buffer_receive, &result, &index_game, client->pointer_list_game);

				bytes_result = send_validated_message(temporary_pointer, buffer_send, client->socket_fd);

				if(bytes_result == -1){

					printf("Error, borken connection: unable to send information to client. Closing connection\n");

					quit = true;

				}

				memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive, 0, sizeof(buffer_receive));

				if(result == 2){ //we need to change this, this is a magical number

					//creating a game, meaning that the client is going to have to wait until someone enters the game that he or she created

					pthread_mutex_lock(&mutex_game_list);

		                                while((client->pointer_list_game + index_game)->second_player == -1 && !timed_out){

                		                        int rc = pthread_cond_timedwait(&((client->pointer_list_game + index_game)->game_condition), &mutex_game_list, &ts);

	                        	                if(rc == ETIMEDOUT){

	                                	                timed_out = 1;

	                                        	}

	        	                        }

	                                pthread_mutex_unlock(&mutex_game_list);

	                                if(timed_out == 1 || (client->pointer_list_game + index_game)->second_player == -1){

						temporary_pointer = "0|3|Game not found: time expired";

	                                }else{

						//a game was found

						temporary_pointer = "0|1|Game was found";

	                                }


				}else{

					//in the other cases, we have already sent a message at the beginning, because the return string of create_game is already sent to the client and has already followed its own internal protocol game logic.
					//this is for specific cases, when no games were found or a game was found and now the client is able to play the game.
					continue;

				}

				break;


			case RANDOM_MESSAGE:

				temporary_pointer = "2|0|Server has recevied random";

				break;

			case QUIT:

				temporary_pointer = "1|0|Server received quit statement, goodbye";

				quit = true;

				break;

			case MENU_PREPERATION:

				//we are going to send the file descriptor of the current client aswell as the second player that has decided to enter the game

				//we are going to have to perform some trype of concatination

				//first number is 3 (GAME_PLAY)

				//sense we want to shoe the menu to the client, we are going to have to send 7: MENU

				temporary_pointer = "3|7|";

				char temporary_buffer[BUFFER_SIZE] = "3|7|";

				counter = 4;

				counter = number_to_char((temporary_buffer + counter), client->socket_fd);

				counter++;

				temporary_buffer[counter] = '|';

				counter++;

				counter = number_to_char((temporary_buffer + counter), ((client->pointer_list_game) + index_game)->second_player);

				counter = 0;

				temporary_pointer = temporary_buffer;

				break;

			default:


				temporary_pointer = "4|0|error, option not valid";
				break;


		}


		bytes_result = send_validated_message(temporary_pointer, buffer_send, client->socket_fd);

		if(bytes_result == -1){

			printf("Error: closing connection with client\n");

			quit = true;

		}

		printf("The server has succesfully sent the following message %s\n", buffer_send);


	}

	if(((client->pointer_list_game) + index_game)->first_player !=-1){

		//we need to set this to -1 so that other players can use this slot in order to play/create a game

              ((client->pointer_list_game) + index_game)->first_player = -1;

		//we have not done menu or access to gamepay yet to the client-server model, so second_plkayer will be untouched for now


	}


	pthread_mutex_lock(&mutex_thread_counter);

		counter_thread --;

	pthread_mutex_unlock(&mutex_thread_counter);

	close(client->socket_fd);


	free(client);

	return NULL;


}


void initilizeGames(game_struct_players* game_list){

	for(int i =0;i<MAX_GAMES_SIZE;i++){

		(game_list+i)->first_player = -1;

		(game_list+i)->second_player = -1;

		memset((game_list+i)->name,0,sizeof((game_list+i)->name));

		(game_list+i)->game_id = -1;

		pthread_cond_init(&(game_list+i)->game_condition,NULL);

	}

}

int main()
{

	int server_file_descriptor = 0, new_socket = 0, opt = 1, port_number = 8080;

	struct sockaddr_in address;

	memset(&address,0, sizeof(address));

	pthread_t thread_clients[MAX_GAMES_SIZE*2];

	game_struct_players game_list[MAX_GAMES_SIZE];

	socklen_t addrlen = sizeof(address);

	server_file_descriptor = socket(AF_INET, SOCK_STREAM,0);


	initilizeGames(game_list);

	pthread_mutex_init(&mutex_game_list,NULL);

	pthread_mutex_init(&mutex_thread_counter,NULL);

	if(setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){

		printf("for some reason there was an error with the configuration of the socket: SO_REUSEADDR\n");

		return -1;

	}

	if(setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))){

		printf("for some reason there was an error with the configuration of the socket: SO_REUSEPORT\n");

		return -1;

	}


	address.sin_family = AF_INET;

	address.sin_addr.s_addr = INADDR_ANY;


	address.sin_port = htons(port_number);

	if (bind(server_file_descriptor, (struct sockaddr*)&address,sizeof(address))< 0) {

	        perror("bind failed");

		return -1;//i do not really know what exit(EXIT_FAILURE) really does, yes it makes the main thread finish execution, but does it behave the same as a return statement

	}

	listen(server_file_descriptor,2);

	printf("----------SERVER-----------\n");


	while(1){

		printf("SERVER IS WAITING\n");

		new_socket = accept(server_file_descriptor, (struct sockaddr*)&address,(socklen_t*)&addrlen);

		if(new_socket == -1){

			printf("Error: server was not able to accept the incoming client: %s\n", strerror(errno));

			continue;

		}

		struct_client* new_client = malloc(sizeof(struct_client));

		new_client->socket_fd = new_socket;

		new_client->pointer_list_game = game_list;

		pthread_mutex_lock(&mutex_thread_counter);

		if(counter_thread>=MAX_CLIENT_THREADS){


			printf("The server is full\n");

			close(new_socket);

			pthread_mutex_unlock(&mutex_thread_counter);

			free(new_client);

			continue;

		}

		int current_index = counter_thread;

		counter_thread = counter_thread + 1;

		pthread_mutex_unlock(&mutex_thread_counter);

		if(pthread_create(&thread_clients[current_index], NULL, &handle_client,(void*)new_client ) !=0){

			printf("Error on creating the thread\n");

			close(new_socket);

			pthread_mutex_lock(&mutex_thread_counter);

			counter_thread = counter_thread - 1;

			pthread_mutex_unlock(&mutex_thread_counter);

			free(new_client);


		}




	}

	for(int i = 0;i<MAX_GAMES_SIZE;i++){

		pthread_cond_destroy(&(game_list[i].game_condition));

	}


	pthread_mutex_destroy(&mutex_game_list);

	pthread_mutex_destroy(&mutex_thread_counter);


	close(server_file_descriptor);
}
