#include "server_logic.h"
#include "server_send_data.h"

void time_init(struct timespec* ts, int time_experation){

	clock_gettime(CLOCK_REALTIME, ts);

	ts->tv_sec +=time_experation;

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
	                	       	return protocol_string_holder[FOUND_GAME];

	        	        }else if((game_list+i)->game_id==-1){

        	        	        (game_list+i)->game_id = i;
                	        	(game_list+i)->player_id[0] = temporary_fd;
	                	        *(result_function) = 2;
        	                	*(index_game) = i;
	                	        pthread_mutex_unlock(&mutex_game_list);
					return protocol_string_holder[CREATED_GAME];
	                	}

	        pthread_mutex_unlock(&mutex_game_list);

        }



        *(result_function) = 3 ;
        *(index_game) = -1;



        return protocol_string_holder[GAMES_OCCUPIED];

}



bool validate_message_length(char* buffer_message, char* buffer_error){

        if(BUFFER_SIZE <= strlen(buffer_message)){

		snprintf(buffer_error, sizeof(buffer_error), "%s", error_string_holder[BUFF_OVF]);

                return false;

        }else{

                return true;

        }

}

bool validate_message_structure(char* buffer_message, char* buffer_error){

	if(buffer_message == NULL){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[NULL_MESS]);

		return false;

	}

	char* first_sep=strchr(buffer_message, '|');

	if(first_sep ==NULL){

		snprintf(buffer_error, BUFFER_SIZE, error_string_holder[STRUCT_FIRST], buffer_message);

		return false;

	}

	char* second_sep = strchr(first_sep + 1, '|');

	if(second_sep == NULL){

		snprintf(buffer_error, BUFFER_SIZE, error_string_holder[STRUCT_SECOND], buffer_message);

		return false;

	}

	return true;

}

bool validate_message_numbers(char* buffer_message, char* buffer_error){

	char* temp_ptr = buffer_message;

	char* save_ptr = NULL;

	int first_number_int = -1;

	int second_number_int = -1;

	char* endptr = NULL;
	if(buffer_message == NULL){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[NULL_MESS]);

		return false;
	}


	char* first_number = strtok_r(temp_ptr, '|', &saveptr);

	if(first_number == NULL){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[BUFF_PROT_FIRST]);

		return false;

	}

	endptr = strch(temp_ptr, "|");

	first_number_int = (int)strol(first_number,&endptr,10);

	temp_ptr = temp_ptr + endptr + 1;//temp_ptr is pointing to character |, we need to add one more position to the pointer in order to start the next part of the protocol number (second protocol number)


	char* second_number = strtok_r (temp_ptr, '|', &saveptr);

	if(second_number == NULL){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[BUFF_PROT_SECOND]);

		return false;

	}

	endptr = strch(temp_ptr, "|");

	second_number_int = (int)strol(second_number, &endptr,10);


	if(first_number_int<0 || first_number_int>10){

		snprintf(buffer_error, BUFFER_SIZE, error_string_holder[PROT_FIRST_VALUE]);

	}


	if(second_number_int<0||second_number_int>10){

		snprintf(buffer_error,BUFFER_SIZE,error_string_holde[PROT_SECOND_VALUE], )

		return false;

	}

	snprintf(buffer_message, BUFFER_SIZE, protocol_string_holder[MENU_PREPERATION],first_number_int, second_number_int);

	return true;

}



ssize_t send_validated_message(const char* temporary_pointer, char* buffer_message, char* buffer_error, int client_file_descriptor){

        ssize_t result = 0;


        if(!validate_message_length(buffer_message, buffer_error)){

		return -1;

	}

	if(!validate_message_numbers(buffer_message,buffer_error)){

		return -1;

	}

	if(validate_message_structure(buffer_message, buffer_error)){

		return -1;

	}


	snprintf(buffer_message, sizeof(buffer_message), "%s", temporary_pointer);

        result = send_framed_message(client_file_descriptor, buffer_message, buffer_error, (uint32_t)strlen(temporary_pointer));


        return result;

}


ssize_t receive_validated_message(char* buffer_message,char* buffer_error, int client_file_descriptor){

        ssize_t result_bytes_receive = receive_framed_message(client_file_descriptor, buffer_message, buffer_error, (ssize_t) BUFFER_SIZE);

        if(result_bytes_receive == -1){

		return -1;

        }


	if(!validate_message_length(buffer_message, buffer_error)){

		return -1;

	}

	if(!validate_message_structure(buffer_message, buffer_error)){

		return -1;

	}

	if(validate_message_numbers(buffer_message, buffer_error)){

		return -1;

	}

        return result_bytes_receive;

}

void switch_game_player_position(game_struct_players* list_game_pointer, int* index_player){

	pthread_mutex_lock(&mutex_game_list);

		list_game_pointer->player_id[0] = list_game_pointer->player_id[1];

		list_game_pointer->ready_player[0] = list_game_pointer->ready_player[1];

	pthread_mutex_unlock(&mutex_game_list);

	*(index_player) = 0;

}
void eliminate_game_slot(void* arg, int* index_game, int* index_player){

	struct_client* fast_pointer =(struct_client*)arg;

	if(!fast_pointer)return;
	if (!fast_pointer->pointer_list_game) return;
	if (*(index_game) < 0 || *(index_game) >= MAX_GAMES_SIZE) return;
	if (*(index_player) < 0 || *(index_player) > 1) return;

	game_struct_players* temp_pointer = (fast_pointer->pointer_list_game) + *(index_game);

		temp_pointer->player_id[*(index_player) & 1] = -1;

		temp_pointer->ready_player[*(index_player) & 1] = false;

		if(temp_pointer->player_id[*(index_player) ^ 1] == -1){

			temp_pointer->game_id = -1;

		}

		pthread_cond_signal(&(temp_pointer->game_condition));


}

int wait_signal_cond(game_struct_players* list_game_pointer, int index_player, struct timespec* ts, int time_exp){

	time_init(ts, time_exp);

	int timed_out = 0;

	pthread_mutex_lock(&mutex_game_list);

         while(list_game_pointer->ready_player[index_player ^ 1] == false && !timed_out){

         	int rc = pthread_cond_timedwait((&list_game_pointer->game_condition), &mutex_game_list, ts);

	         if(list_game_pointer->player_id[index_player ^ 1] == -1){

	         	timed_out = 1;

	         }

	         if(rc == ETIMEDOUT){

		                timed_out = 1;

		}

	}

	pthread_mutex_unlock(&mutex_game_list);

	return timed_out; 

}

char* waiting_for_player(struct_client* client, int* index_game,int* index_player, int time_experation, struct timespec* ts, int* counter,char buffer_receive[], int* result_function, int* timed_out){

	time_init(ts,time_experation);

	*(counter) = 4;

	int play = buffer_receive[*(counter)] - '0';//we receive what the current client wants to do after viewing menu information

	game_struct_players* list_game_pointer = ((client->pointer_list_game) + *(index_game)); //simplify pointer arithmetic

	pthread_mutex_lock(&mutex_game_list);

		list_game_pointer->ready_player[*(index_player) & 1] = (bool)play;

		if(!play){

			list_game_pointer->player_id[*(index_player) & 1] = -1;

		}

		pthread_cond_signal(&(list_game_pointer->game_condition));


	if(play == false){

		eliminate_game_slot(client, index_game, index_player);

		pthread_mutex_unlock(&mutex_game_list);

		*(result_function) = 1;

		return protocol_string_holder[PL_QUIT];


	}else{


		pthread_mutex_unlock(&mutex_game_list);

		*(timed_out) = wait_signal_cond(list_game_pointer, (*index_player), ts, time_experation);

		if(*(timed_out) == 1 || list_game_pointer->ready_player[*(index_player) ^ 1] == false){

			switch_game_player_position(client->pointer_list_game,index_player);

			*(result_function) = 2;

			return protocol_string_holder[OT_QUIT];

		}else{

			*(result_function) = 3;

			return protocol_string_holder[BT_READY];

		}

	}

}



void handlePE(size_t* result, char buffer_receive[],char buffer_error[], bool* quit, int* first_number){


	printf("We have received the following number of bytes: %d\n",(int)*(result));

	//the following line is going to be presented when we have defined some type of specific Error typedef struct:	printf("Errno is giving us the following value: , and the string to this error is the following: \n", (int)(errno), strerror(errno));

	if(*(result) == -1){

		printf("Error message: %s\n", buffer_error);

		printf("An error happend, we are going to close the connection\n");

		//here we are going to have to set the numbers specificly for quitting, keep in mind that for now, when someone quits the other endpoint is going to know due to a event or time experation (this is bad but we will fix later)

		*(first_number) = QUIT;

		*(quit) = true;

	}else{

		printf("No error has occured, message from client: %s\n", buffer_receive);

		printf("Setting first number to what the message has sent over\n");

		*(first_number) = buffer_receive[0] - '0';

	}


}


int menu_preperation_validation(struct_client* client, int index, char* temporary_buffer, char* buffer_error){

	if(index<0||index_game>=MAX_GAMES_SIZE){

		snprintf(buffer_error, sizeof(buffer_error), error_string_holder[MENU_INDEX]);

		return -1;

	}

	int p0 = client->pointer_list_game[index_game].player_id[0];

	int p1 = client->poitner_list_game[index_game].player_id[1];

	int result = snprintf(temporary_buffer, sizeof(temporary_buffer), protocol_string_holder[MENU_PREPERATION], p0, p1); //in the future we are going to have to change this, using hardcoded strings is not good

	return result;
}
