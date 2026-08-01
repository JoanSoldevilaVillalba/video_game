#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "server_send_data.h"

#define BUFFER_SIZE 64

#define MAX_GAMES_SIZE 2


typedef struct{

int first_player;

int second_player;

char name[62];

int game_id;

pthread_cond_t game_condition;

}game_struct_players;



pthread_mutex_t mutex_game_list;
pthread_cond_t conditional_variable;
char* create_game(int temporary_fd, char* buffer_receive, int* result_function, int* index_game, game_struct_players* game_list){

	int i = 0;

	while(i<MAX_GAMES_SIZE){

		pthread_mutex_lock(&mutex_game_list);

		if((game_list+i)->game_id!=-1 && (game_list+i)->second_player==-1){

			(game_list+i)->second_player = temporary_fd;
			*(result_function) = 2;
			*(index_game) = i;
			pthread_mutex_unlock(&mutex_game_list);
			pthread_cond_signal(&(game_list + i)->game_condition);
			return "1|2|you have succesfuly entered a game";

		}else if((game_list+i)->game_id==-1){

			(game_list+i)->game_id = i;
			(game_list+i)->first_player = temporary_fd;
			*(result_function) = 1;
			*(index_game) = i;
			pthread_mutex_unlock(&mutex_game_list);
			return "1|1|you have succesfuly created a game";

		}else{

			i++;
		}


	pthread_mutex_unlock(&mutex_game_list);


	}

	*(result_function) = 0 ;
	*(index_game) = -1;

	return "1|3|All games are occupied, try again later";

}


typedef struct{

int socket_fd;

game_struct_players* pointer_list_game;

}struct_client;

void* handle_client(void* arg){


	char buffer_receive[BUFFER_SIZE], buffer_send [BUFFER_SIZE];

	int client_game_id = -1, bytes_received = 0, bytes_sent = 0, result = 0, counter = 0, quit = false;


	while(!quit){

		counter = 0;

		int result_function = -1;

		int game_index = -1;

		memset(buffer_receive,0,sizeof(buffer_receive));

		memset(buffer_send,0,sizeof(buffer_send));


		bytes_received = read_all(((struct_client*)arg)->socket_fd, buffer_receive, BUFFER_SIZE-1);

		if(bytes_received>0){

			buffer_receive[bytes_received] = '\0';

		}else{

			printf("Error, we recevied zero bytes\n Client probably disconnected, breaking\n");

			break;

		}


		result = buffer_receive[0] - '0';

		char* temporary_char_pointer= NULL;

		switch(result){


			case 1:

				temporary_char_pointer = create_game(((struct_client*)arg)->socket_fd, buffer_receive,&result_function,&game_index,((struct_client*)arg)->pointer_list_game);

					if(result_function == 1){

						temporary_char_pointer="1|1|game created, waiting for second player";

						strcpy(buffer_send, temporary_char_pointer);

						bytes_sent = send_all(((struct_client*)arg)->socket_fd, buffer_send, BUFFER_SIZE);

						pthread_mutex_lock(&mutex_game_list);


						while((((struct_client*)arg)->pointer_list_game + game_index)->second_player == -1){


								pthread_cond_wait(&(((struct_client*)arg)->pointer_list_game + game_index)->game_condition, &mutex_game_list);

						}


						pthread_mutex_unlock(&mutex_game_list);

						strcpy(buffer_send, "1|2|Second player joined, starting game");

				                send_all(client_data->socket_fd, buffer_send, BUFFER_SIZE);

						continue;

					}


				break;

			case 2:

				temporary_char_pointer = "|1|user wants to quit game";

				quit = true;

				break;

		}

		if(temporary_char_pointer != NULL){

			strcpy(buffer_send, temporary_char_pointer);

			send_all(client_data->socket_fd, buffer_send, BUFFER_SIZE);


		}



		bytes_sent = send_all(((struct_client*)arg)->socket_fd, buffer_send, BUFFER_SIZE);

	}

	close(((struct_client*)arg)->socket_fd);

	free((struct_client*)arg);

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

	int server_file_descriptor = 0, new_socket = 0, opt = 1, port_number = 8080, counter_thread_client = 0;

	struct sockaddr_in address;

	pthread_t thread_clients[MAX_GAMES_SIZE*2];

	game_struct_players game_list[MAX_GAMES_SIZE];

	socklen_t addrlen = sizeof(address);

	server_file_descriptor = socket(AF_INET, SOCK_STREAM,0);


	initilizeGames(game_list);

	pthread_mutex_init(&mutex_game_list,NULL);

	if(setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){

		printf("for some reason there was an error with configuring the socket\n");

		return 2;

	}


	address.sin_family = AF_INET;

	address.sin_addr.s_addr = INADDR_ANY;


	address.sin_port = htons(port_number);

	if (bind(server_file_descriptor, (struct sockaddr*)&address,sizeof(address))< 0) {

	        perror("bind failed");

	        exit(EXIT_FAILURE);
	}

	listen(server_file_descriptor,2);

	printf("----------SERVER-----------\n");


	while(1){

		printf("SERVER IS WAITING\n");

		new_socket = accept(server_file_descriptor, (struct sockaddr*)&address,(socklen_t*)&addrlen);

		struct_client* new_client = malloc(sizeof(struct_client));

		new_client->socket_fd = new_socket;

		new_client->pointer_list_game = game_list;

		pthread_create(&thread_clients[counter_thread_client++], NULL, &handle_client,(void*)new_client );

	}


	pthread_mutex_destroy(&mutex_game_list);




	close(server_file_descriptor);
}
