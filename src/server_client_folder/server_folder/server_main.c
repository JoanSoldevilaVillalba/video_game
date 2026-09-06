#include "server_logic.h"

pthread_mutex_t mutex_game_list;
pthread_mutex_t mutex_thread_counter;
pthread_cond_t conditional_variable;
int counter_thread;

void* handle_client(void* arg){

	struct_client* client = (struct_client*)arg;

	pthread_detach(pthread_self());

	char buffer_receive[64], temporary_buffer[64], buffer_error[64];

	int result = 0, counter = 0, first_number = -1, index_game = -1, index_player = -1, timed_out = 0;

	bool quit = false;

	ssize_t bytes_result = 0;

	struct timespec ts;

	while(!quit){

		memset(buffer_receive, 0, sizeof(buffer_receive));counter = 0 ;


		bytes_result = receive_validated_message(buffer_receive, buffer_error,client->socket_fd);

		handlePE(&bytes_result, buffer_receive, buffer_error, &quit, &first_number);

		if(quit == false){

			//when an error has occured due to protocol or something else, the server is going to ignore what the client has sent over

			first_number = str_to_int(buffer_receive, buffer_error);

			handlePE((ssize_t*)&first_number, buffer_receive, buffer_error, &quit, &first_number);

		}

		counter = 0;


		switch(first_number){


			case ENTERING_CREATING_GAME_STATE:{

				time_init(&ts,TM_EXP_WAIT_GAME);

				timed_out = 0;


				create_game(client->socket_fd,&result, &index_game, client->pointer_list_game, temporary_buffer);

				bytes_result = send_validated_message(temporary_buffer, buffer_error, client->socket_fd);

				handlePE(&bytes_result, temporary_buffer, buffer_error, &quit, &first_number);

				memset(buffer_receive, 0, sizeof(buffer_receive));


				if(!quit){

					if(result == 1){

						index_player = 1;

					}else if(result == 2){

						timed_out = wait_signal_cond((client->pointer_list_game) + index_game ,index_player, &ts, TM_EXP_WAIT_GAME);

	                                	if(timed_out == 1 || (client->pointer_list_game + index_game)->player_id[1] == -1){

							snprintf(temporary_buffer, BUFFER_SIZE, "%s", protocol_string_holder[GAME_EXPERATION]);

	                	                }else{

							snprintf(temporary_buffer, BUFFER_SIZE, "%s",protocol_string_holder[FOUND_GAME]);

							index_player = 0;

		                                }


					}else{

						continue;

					}
				}else{

					snprintf(temporary_buffer, BUFFER_SIZE, "%s", buffer_error);


				}
				break;
				}

			case RANDOM_MESSAGE_STATE:

				snprintf(temporary_buffer, BUFFER_SIZE, "%s", protocol_string_holder[RANDOM_MESSAGE_PROT]);

				break;

			case QUIT_STATE:

				snprintf(temporary_buffer, BUFFER_SIZE, "%s", protocol_string_holder[QUIT_CLIENT]);

				quit = true;

				break;

			case MENU_PREPERATION_STATE:

				bytes_result = menu_preperation_validation(client,index_game, temporary_buffer,buffer_error);

				handlePE(&bytes_result, temporary_buffer, buffer_error, &quit, &first_number);


			    break;

			case WAITING_INIT_STATE:

				waiting_for_player(client, &index_game,&index_player, TM_EXP_WAIT_GAME, &ts, &counter,buffer_receive,  &result, &timed_out, temporary_buffer);

				result = -1;

				break;

			case KEEP_WAITING_STATE:

				timed_out = wait_signal_cond((client->pointer_list_game) + index_game, index_player, &ts, TM_EXP_WAIT_GAME);

				if(timed_out == 1 || ((client->pointer_list_game) + index_game)->ready_player[index_player ^ 1] == false){

					//temporary_pointer ="still no one";

				}else{

					//temporary_pointer = "someone else has enterd our game"; 

				}

				break;TM_EXP_WAIT_GAME

			case PLAY_TIME_STATE:

				break;


			default:


				snprintf(temporary_buffer, sizeof(temporary_buffer), "%s", protocol_string_holder[INVALID_OPT]);

				break;


		}


		bytes_result = send_validated_message(temporary_buffer,buffer_error, client->socket_fd);

		handlePE(&bytes_result, temporary_buffer,buffer_error, &quit, &first_number);


		printf("The server has succesfully sent the following message %s\n", temporary_buffer);


	}


	pthread_mutex_lock(&mutex_thread_counter);

		if(counter_thread>0){

			counter_thread --;

		}


	if(index_game != -1 && index_player != -1){

		eliminate_game_slot(client,&index_game, &index_player);

	}

	pthread_mutex_unlock(&mutex_thread_counter);


	close(client->socket_fd);


	free(client);

	return NULL;


}


void initilizeGames(game_struct_players* game_list){

	for(int i =0;i<MAX_GAMES_SIZE;i++){

		(game_list+i)->player_id[0] = -1;

		(game_list+i)->player_id[1] = -1;

		memset((game_list+i)->name,0,sizeof((game_list+i)->name));

		(game_list+i)->game_id = -1;

		pthread_cond_init(&(game_list+i)->game_condition,NULL);

		(game_list+i)->ready_player[0] = false;

		(game_list+i)->ready_player[1] = false;

	}

}

int main()
{
	signal(SIGPIPE, SIG_IGN);

	int server_file_descriptor = 0, new_socket = 0, opt = 1, port_number = 8080;

	struct sockaddr_in address;

	memset(&address,0, sizeof(address));

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

		return -1;

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

		if(counter_thread + 1>=MAX_CLIENT_THREADS){


			printf("The server is full\n");

			close(new_socket);

			pthread_mutex_unlock(&mutex_thread_counter);

			free(new_client);

			continue;

		}

		counter_thread = counter_thread + 1;

		pthread_mutex_unlock(&mutex_thread_counter);

		pthread_t temp_thread;
		if(pthread_create(&temp_thread, NULL, &handle_client, (void*)new_client ) != 0){
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

	return 0;
}
