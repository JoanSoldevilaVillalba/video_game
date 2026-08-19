#include "server_logic.h"
#include "server_send_data.h"

void time_init(struct timespec* ts, int time_experation){

	clock_gettime(CLOCK_REALTIME, ts);

	ts.tv_sec +=time_experation;

}

void reverse(char* pointer){

        ssize_t length = strlen(pointer);

        for(ssize_t i = 0;i<length/2;i++){

                char temp = *(pointer + i);

                *(pointer + i) = *(pointer + length - 1 - i);

                *(pointer + length - 1 - i) = temp;

        }

}

int int_to_str(char *dst, ssize_t dst_size, int value){

	if(!dst||dst_size == 0){

		 return -1;
	}

	int n = snprintf(dst, dst_size, "%d", value);

	if(n<0 || (ssize_t)n>=dst_size){

		return -1;

	}

	return n;

}


char* create_game(int temporary_fd, int* result_function, int* index_game, game_struct_players* game_list){

        for(int i = 0;i<MAX_GAMES_SIZE;i++){

                pthread_mutex_lock(&mutex_game_list);

                if((game_list+i)->game_id!=-1 && (game_list+i)->player_id[1]==-1){

                        (game_list+i)->player_id[1] = temporary_fd;
                        *(result_function) = 1;
                        *(index_game) = i;
                        pthread_cond_signal(&(game_list + i)->game_condition);
                        pthread_mutex_unlock(&mutex_game_list);
                        return "0|1|you have succesfuly entered a game";

                }else if((game_list+i)->game_id==-1){

                        (game_list+i)->game_id = i;
                        (game_list+i)->player_id[0] = temporary_fd;
                        *(result_function) = 2;
                        *(index_game) = i;
                        pthread_mutex_unlock(&mutex_game_list);
                        return "0|2|you have succesfuly created a game";

                }

        pthread_mutex_unlock(&mutex_game_list);

        }



        *(result_function) = 3 ;
        *(index_game) = -1;

        return "0|3|All games are occupied, try again later";

}



bool validate_message_length(const char* temporary_pointer){

        if(BUFFER_SIZE <= strlen(temporary_pointer)){

                return false;

        }else{

                return true;

        }

}



ssize_t send_validated_message(const char* temporary_pointer, char buffer[BUFFER_SIZE], int client_file_descriptor){

        bool correct_length = validate_message_length(temporary_pointer);

        ssize_t result = 0;

        if(correct_length == true){

                strncpy(buffer, temporary_pointer, strlen(temporary_pointer));

                buffer[strlen(temporary_pointer)] = '\0';

                result = send_framed_message(client_file_descriptor, buffer, (uint32_t)strlen(temporary_pointer));

                if(result == -1){

                        printf("Unable to send message to server\n");

                        result = -1;

                }

        }else{

                printf("Error, the following message exceeds the BUFFER_SIZE limit: %s\n", temporary_pointer);

                result = -2;

        }



        return result;

}


ssize_t receive_validated_message(char buffer[BUFFER_SIZE], int client_file_descriptor){

        ssize_t result_bytes_receive = receive_framed_message(client_file_descriptor, buffer, (ssize_t) BUFFER_SIZE);

        const char* temporary_pointer = buffer;

        bool validated = validate_message_length(temporary_pointer);

        if(result_bytes_receive == -1 || !validated){

                return -1;

        }

        return result_bytes_receive;

}

void switch_game_player_position(int* list_game_pointer int* index_game, int* index_player){

	//the folloiwng is only necessary if the current client is in the second position of the array

	if(!*(index_game)) return; //if the value pointed by index_game is equal to zero or false (zero is false), we do not need to perform a switch

	if(!(list_game_pointer))return;

	if(*(index_player)<0 || *(index_player)<0)return;

	pthread_mutex_lock(&mutex_game_list);

		(list_game_pointer->player_id[0] = list_game_pointer->player_id[1];

		(list_game_pointer->ready_player[0] = list_game_pointer->ready_player[1];

	pthread_mutex_unlock(&mutex_game_list);

	*(index_player) = 0;

}
void eliminate_game_slot(void* arg, int* index_game, int* index_player){

	struct_client* fast_pointer =(struct_client*)arg;

	if(!fast_pointer){

		return;

	}

	if (!fast_pointer->pointer_list_game) return;
	if (*(index_game) < 0 || *(index_game) >= MAX_GAMES_SIZE) return;
	if (*(index_player) < 0 || *(index_player) > 1) return;

	int* temp_pointer = (fast_pointer->pointer_list_game) + index_game;

	pthread_mutex_lock(&mutex_game_list);

	temp_pointer->player_id[index_player & 1] = -1;

	temp_pointer->ready_player[index_player & 1] = false;

	if(temp_pointer->player_id[index_player ^ 1] == -1){

		temp_pointer->game_id = -1;

	}

	pthread_cond_signal(&(temp_pointer->game_condition));

	pthread_mutex_unlock(&mutex_game_list);

}

//list_game_pointer is the same as the following: client->pointer_list_game

int wait_signal_cond(int* list_game_pointer, int index_player, struct timespec* ts, int time_exp){

	time_init(ts, time_exp);

	pthread_mutex_lock(&mutex_game_list);

	int timed_out = 0;

         while(list_game_pointer->ready_player[index_player ^ 1] == false && !timed_out){

         	int rc = pthread_cond_timedwait(&list_game_pointer->game_condition), &mutex_game_list, ts);

	         if(list_game_pointer->player_id[index_player ^ 1] == -1){

	         	timed_out = 1;

	         }

	         if(rc == ETIMEDOUT){

		                timed_out = 1;

		}

	}

	pthread_mutex_unlock(&mutex_game_list);

	return timed_out; //when timed_out is equal to 1, that means that the other player has decided to quit or time experation

}

char* waiting_for_player(struct struct_client* client, int* index_game,int* index_player, int time_experation, struct timespec* ts, int* counter,char buffer_receive[], int* result_function, int* timed_out){

	time_init(ts,time_experation);

	*(counter) = 4;

	int play = buffer_receive[counter] - '0';//we receive what the current client wants to do after viewing menu information

	int* list_game_pointer = ((client->pointer_list_game) + index_game); //simplify pointer arithmetic

	pthread_mutex_lock(&mutex_game_list);

		list_game_pointer->ready_player[index_player & 1] = (bool)play;

		if(!play){

			list_game_pointer->player_id[index_player & 1] = -1;//if we do not want to play we set index_player id to -1, liberating this slot for another player who is currently looking

		}

		pthread_cond_signal(&(list_game_pointer->game_condition)); //after changing game_list variables, we send a signal to the other player indicating that we have changed variables

	pthread_mutex_unlock(&mutex_game_list);

	if(play == false){

		eliminate_game_slot(client, index_game, index_player); //we elinate ouersevles fromthe game, 

		*(result_function) = 1 //this player has selected to quit after seeing menu information

		return "1|8|player is quitting";


	}else{


		//in htis case the curretn client wants to continue to play the game

		*(timed_out) = wait_signal_cond(list_game_pointer, (*index_player));

		if(*(timed_out) == 1 || (list_game_pointer->ready_player[index_player ^ 1] == false){

			switch_game_player_position(client,index_game, index_player);

			*(result_function) = 2; // other player quitted after seeing game menu, it is up to the current client if he or she wants  to continue to wait for someone else to enter the game

			return "1|7|other player quit game";

		}else{

			*(result_function) = 3; //both players after reveiwn gmenu information, they have both decided to continue to play, main game loop is going to start soon

			return "3|10|other player ready";

		}

	}

}
