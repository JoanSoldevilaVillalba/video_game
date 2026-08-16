#include "server.h"

#include "server_send_data.h"

#include "server_logic.h"

pthread_mutex_t mutex_game_list;
pthread_mutex_t mutex_thread_counter;
pthread_cond_t conditional_variable;
int counter_thread;

void* handle_client(void* arg){

	struct_client* client = (struct_client*)arg;

	pthread_detach(pthread_self());

	char buffer_receive[BUFFER_SIZE], buffer_send [BUFFER_SIZE], temporary_buffer[BUFFER_SIZE];

	int result = 0, counter = 0, first_number = -1, index_game = -1, index_player = -1;

	bool quit = false;

	ssize_t bytes_result = 0;

	const char* temporary_pointer = NULL;

	while(!quit){

		memset(buffer_receive, 0, sizeof(buffer_receive)); memset(buffer_send, 0, sizeof(buffer_send)); temporary_pointer = NULL; counter = 0 ;

		bytes_result = receive_validated_message(buffer_receive, client->socket_fd);

		if(bytes_result == -1){

			printf("Error, closing the connection with client, %s\n", strerror(errno));

			quit = true;

			break;

		}

		printf("We have received the following message from the client: %s\n", buffer_receive);

		counter = 0;

		first_number = buffer_receive[counter] - '0';

		switch(first_number){


			case ENTERING_CREATING_GAME:{

				struct timespec ts;

				clock_gettime(CLOCK_REALTIME, &ts);

				ts.tv_sec +=5;

				int timed_out = 0;

				memset(buffer_send, 0, sizeof(buffer_send));

				temporary_pointer = create_game(client->socket_fd,&result, &index_game, client->pointer_list_game);

				bytes_result = send_validated_message(temporary_pointer, buffer_send, client->socket_fd);

				if(bytes_result == -1){

					printf("Error, borken connection: unable to send information to client. Closing connection\n");

					quit = true;

				}

				memset(buffer_send, 0, sizeof(buffer_send)); memset(buffer_receive, 0, sizeof(buffer_receive));

				if(result == 1){

					index_player = 1;

				}

				if(result == 2){

					pthread_mutex_lock(&mutex_game_list);

		                                while((client->pointer_list_game + index_game)->player_id[1] == -1 && !timed_out){

                		                        int rc = pthread_cond_timedwait(&((client->pointer_list_game + index_game)->game_condition), &mutex_game_list, &ts);

	                        	                if(rc == ETIMEDOUT){

	                                	                timed_out = 1;

	                                        	}

	        	                        }

	                                pthread_mutex_unlock(&mutex_game_list);

	                                if(timed_out == 1 || (client->pointer_list_game + index_game)->player_id[1] == -1){

						temporary_pointer = "0|3|Game not found: time expired";

	                                }else{

						temporary_pointer = "0|1|Game was found";

						index_player = 0;

	                                }


				}else{

					continue;

				}

				break;
				}

			case RANDOM_MESSAGE:

				temporary_pointer = "2|0|Server has recevied random";

				break;

			case QUIT:

				temporary_pointer = "1|0|Server received quit statement, goodbye";

				quit = true;

				break;

			case MENU_PREPERATION:

			    if (index_game < 0 || index_game >= MAX_GAMES_SIZE) {
				        strncpy(temporary_buffer, "4|0|error, invalid game index", BUFFER_SIZE);
				        temporary_buffer[BUFFER_SIZE-1] = '\0';
			    }else {
				        int p0 = client->pointer_list_game[index_game].player_id[0];
				        int p1 = client->pointer_list_game[index_game].player_id[1];
				        int n = snprintf(temporary_buffer, sizeof temporary_buffer, "3|7|%d|%d", p0, p1);
				        if (n < 0 || n >= (int)sizeof temporary_buffer) {
				            strncpy(temporary_buffer, "1|0|Server wants to quit, Goodbye", BUFFER_SIZE);
				            temporary_buffer[BUFFER_SIZE-1] = '\0';
				        }
			    }
				temporary_pointer=temporary_buffer;

			    break;

			case WAITING_INIT:{

				struct timespec ts;

				clock_gettime(CLOCK_REALTIME, &ts);

				ts.tv_sec +=120;

				int timed_out = 0;

				counter = 4;

				int play = buffer_receive[counter] - '0';

				pthread_mutex_lock(&mutex_game_list);

					(client->pointer_list_game + index_game)->ready_player[index_player & 1] = (bool)play;

					if(!play){

						(client->pointer_list_game + index_game)->player_id[index_player & 1] = -1;

					}
					pthread_cond_signal(&(client->pointer_list_game + index_game)->game_condition);

				pthread_mutex_unlock(&mutex_game_list);

				if(play == false){

					temporary_pointer = "1|8|player is quitting";

					eliminate_game_slot(client, index_game, index_player);

				}else{

					pthread_mutex_lock(&mutex_game_list);

						while((client->pointer_list_game + index_game)->ready_player[index_player ^ 1] == false && !timed_out){

							int rc = pthread_cond_timedwait(&((client->pointer_list_game + index_game)->game_condition), &mutex_game_list, &ts);

							if((client->pointer_list_game + index_game)->player_id[index_player ^ 1] == -1){

								timed_out = 1;

							}

							if(rc == ETIMEDOUT){

								timed_out = 1;

							}

						}

					pthread_mutex_unlock(&mutex_game_list);

					if(timed_out == 1 || (client-> pointer_list_game + index_game)->ready_player[index_player ^ 1] == false){

						temporary_pointer = "1|7|other player quit game";

						//if the other player quit, we are going to have to switch the currents clients poisition within the curretn game slot, only if the he or she found the game (index_player = 1), not when he or she created the game

						switch_game_player_position(client,index_game, &index_player);

					}else{

						temporary_pointer = "3|10|other player ready";

					}

				}

				break;
				}

			case KEEP_WAITING:

				//after the game was created, menu information was sent, but the other player still decided to quit at the end, this curretn client has the possiblity to continue to wait until someone else deicdes to enter its game

				break;

			case PLAYTIME:

				//after both players have accepted, we are going to have to switch from TCP to UDP, define another protocol that the client is going to have to send to the server indicating waht movements the player is doing (left, right, shooting ...)

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


	pthread_mutex_lock(&mutex_thread_counter);

		if(counter_thread>0){

			counter_thread --;

		}

	pthread_mutex_unlock(&mutex_thread_counter);

	if(index_game != -1 && index_player != -1){

		eliminate_game_slot(client,index_game, index_player);

	}

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
}
