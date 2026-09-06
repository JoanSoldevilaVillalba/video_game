#include "server_logic.h"

const char* protocol_string_holder[] ={
[FOUND_GAME] = "0|1|you have found a game",
[CREATED_GAME] = "0|2|you have created a game",
[GAMES_OCCUPIED] = "0|3|all games occupied",
[GAME_EXPERATION] = "0|4|Game not found: time expired",
[GAME_NO_SCND_PLAYER] ="0|5|Second player for your match is not found yet",
[RANDOM_MESSAGE_PROT] = "2|1|this is response to random message",
[QUIT_CLIENT] = "1|1|server received quit statement, goodbye",
[MENU_PREPERATION_PROT] = "3|7|%d|%d",
[PL_QUIT] = "1|8|player is quitting",
[OT_QUIT] = "1|4|other player is quitting",
[BT_READY] = "3|3|other player ready",
[INVALID_OPT] = "4|0|error, option not valid",
};

const char* error_string_holder[] = {
    [SYS_POLL]              = "Error in event driven poll syscall (errno str):%s",
    [TIME_EXPIRED_POLL]     = "Error, time out expired for poll event",
    [SYS_RECV]              = "Error, recv syscall went wrong (errno str):%s",
    [SYS_SEND]              = "Error, send syscall went wrong (errno str): %s",
    [INIT_SEND_LEN]         = "Error, amount of bytes sent in init message is not equal to 4 bytes",
    [MESS_SEND_LEN]         = "Error, amount of bytes sent of real message is not equal to length/bytes of message",
    [INIT_RECV_LEN]         = "Error, amount of bytes received in init message is not equal to 4 bytes",
    [MESS_RECV_LEN_OVF]     = "Error, amount of bytes recevied in init message is overflowing (>BUFFER_SIZE)",
    [MESS_RECV_LEN]         = "Error, amount of bytes received in real mesage is not equal to length/bytes of init message",
    [BUFF_OVF]              = "Error, message length is too large: overflow",
    [BUFF_STRUCT]           = "Error, message odes not have the correct structure",
    [BUFF_PROT_FIRST]       = "Error, first protocol number was not found",
    [BUFF_PROT_SECOND]      = "Error, second protocol number was not found",
    [MENU_INDEX]            = "Error, game index contains bad value",
    [STRUCT_FIRST]          = "Error, first seperator was not found in the following message: %s",
    [STRUCT_SECOND]         = "Error, second seperator was not found in the following message: %s",
    [NULL_MESS]             = "Error, null message",
    [PROT_FIRST_VALUE]      = "Error, first protocol number is out of range (value not correct)",
    [PROT_SECOND_VALUE]     = "Error, second protocol number is out of range (value not correct)",
    [NO_FIRST_NUMBER]       = "Error, the following message does not contain first protocol number: %s",
    [NO_SECOND_NUMBER]      = "Error, the folloiwng message does not contain second protocol number: %s"
};



void time_init(struct timespec* ts, int time_experation){

	clock_gettime(CLOCK_REALTIME, ts);

	ts->tv_sec +=time_experation;

}

int str_to_int(char* buffer_message, char* buffer_error){

	if(buffer_message == NULL){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[NULL_MESS]);

		return -1;

	}

	char* first = buffer_message;

	char*end = NULL;

	int result = -1;

	const char delimiter = '|';

	end = strchr(first,delimiter);

	if(end == NULL){

		snprintf(buffer_error, BUFFER_SIZE,  error_string_holder[STRUCT_FIRST], buffer_message);

	}


	if(end == first){

		snprintf(buffer_error, BUFFER_SIZE, error_string_holder[NO_FIRST_NUMBER], buffer_message);

	}

	result = (int)strtol(first, &end,10);

	return result;


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


void create_game(int temporary_fd, int* result_function, int* index_game, game_struct_players* game_list, char* buffer_message){

        for(int i = 0;i<MAX_GAMES_SIZE;i++){

                pthread_mutex_lock(&mutex_game_list);

	                	if((game_list+i)->game_id!=-1 && (game_list+i)->player_id[1]==-1){

        	                	(game_list+i)->player_id[1] = temporary_fd;
	                	        *(result_function) = 1;
	                        	*(index_game) = i;
		                        pthread_cond_signal(&(game_list + i)->game_condition);
	        	                pthread_mutex_unlock(&mutex_game_list);
	                	       	snprintf(buffer_message,BUFFER_SIZE, "%s", protocol_string_holder[FOUND_GAME]);



	        	        }else if((game_list+i)->game_id==-1){

        	        	        (game_list+i)->game_id = i;
                	        	(game_list+i)->player_id[0] = temporary_fd;
	                	        *(result_function) = 2;
        	                	*(index_game) = i;
	                	        pthread_mutex_unlock(&mutex_game_list);
					snprintf(buffer_message, BUFFER_SIZE, "%s", protocol_string_holder[CREATED_GAME]);


	                	}

	        pthread_mutex_unlock(&mutex_game_list);

        }



        *(result_function) = 3 ;
        *(index_game) = -1;



        snprintf(buffer_message, BUFFER_SIZE, "%s", protocol_string_holder[GAMES_OCCUPIED]);

}



bool validate_message_length(char* buffer_message, char* buffer_error){

        if(BUFFER_SIZE <= strlen(buffer_message)){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[BUFF_OVF]);

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

        char* first = buffer_message;

        char* end = NULL;

        const char delimiter = '|';

        int first_number = -1, second_number = -1;

        if(buffer_message == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[NULL_MESS]);

                return false;

        }


        end = strchr(first, delimiter);

        if(end == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[STRUCT_FIRST]);

                return false;

        }

        if(first == end){

                snprintf(buffer_error, BUFFER_SIZE,error_string_holder[NO_FIRST_NUMBER], buffer_message);

                return false;

        }

        first_number = (int)strtol(first, &end,10);



        first = end + 1;

        end = first;

        end = strchr(first, delimiter);

        if(end == NULL){

                snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[STRUCT_SECOND]);

                return false;

        }

        if(end == first){

                snprintf(buffer_error, BUFFER_SIZE,error_string_holder[NO_SECOND_NUMBER], buffer_message);

                return false;

        }

        second_number = (int)strtol(first, &end, 10);

        if(first_number<0 || first_number>10){

                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[PROT_FIRST_VALUE]);

                return false;

        }


        if(second_number<0||second_number>10){

                snprintf(buffer_error,BUFFER_SIZE,"%s",error_string_holder[PROT_SECOND_VALUE]);

                return false;

        }

        return true;

}



ssize_t send_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor){

        ssize_t result = 0;


        if(!validate_message_length(buffer_message, buffer_error)){

		return -1;

	}

	if(!validate_message_numbers(buffer_message,buffer_error)){

		return -1;

	}

	if(!validate_message_structure(buffer_message, buffer_error)){

		return -1;

	}


        result = send_framed_message(client_file_descriptor, buffer_message, buffer_error, (uint32_t)strlen(buffer_message));


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

	if(!validate_message_numbers(buffer_message, buffer_error)){

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

void waiting_for_player(struct_client* client, int* index_game,int* index_player, int time_experation, struct timespec* ts, int* counter,char buffer_receive[], int* result_function, int* timed_out, char* buffer_message){

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

		snprintf(buffer_message, BUFFER_SIZE, "%s", protocol_string_holder[PL_QUIT]);


	}else{


		pthread_mutex_unlock(&mutex_game_list);

		*(timed_out) = wait_signal_cond(list_game_pointer, (*index_player), ts, time_experation);

		if(*(timed_out) == 1 || list_game_pointer->ready_player[*(index_player) ^ 1] == false){

			switch_game_player_position(client->pointer_list_game,index_player);

			*(result_function) = 2;

			snprintf(buffer_message, BUFFER_SIZE, "%s", protocol_string_holder[OT_QUIT]);

		}else{

			*(result_function) = 3;

			snprintf(buffer_message, BUFFER_SIZE, "%s", protocol_string_holder[BT_READY]);

		}

	}

}



void handlePE(ssize_t* result, char buffer_receive[],char buffer_error[], bool* quit, int* first_number){


	printf("\n----- ERROR HANDLER ---- \n");

	printf("We have received the following number of bytes: %d\n",(int)*(result));

	//the following line is going to be presented when we have defined some type of specific Error typedef struct:	printf("Errno is giving us the following value: , and the string to this error is the following: \n", (int)(errno), strerror(errno));

	if(*(result) == -1){

		if(buffer_error == NULL){

			printf("There was an error, but buffer_error does not have anyting inside\n");

		}else{

			printf("Error message: %s\n", buffer_error);

		}

		printf("An error happend, we are going to close the connection\n");

		//here we are going to have to set the numbers specificly for quitting, keep in mind that for now, when someone quits the other endpoint is going to know due to a event or time experation (this is bad but we will fix later)

		*(first_number) = QUIT_STATE;

		*(quit) = true;

	}else{

		printf("No error has occured, message from client: %s\n", buffer_receive);

		//printf("Setting first number to what the message has sent over\n");

		/*(first_number) = buffer_receive[0] - '0';*/

	}


}


int menu_preperation_validation(struct_client* client, int index, char* temporary_buffer, char* buffer_error){

	if(index<0||index>=MAX_GAMES_SIZE){

		snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[MENU_INDEX]);

		return -1;

	}

	int p0 = client->pointer_list_game[index].player_id[0];

	int p1 = client->pointer_list_game[index].player_id[1];

	int result = snprintf(temporary_buffer, BUFFER_SIZE, protocol_string_holder[MENU_PREPERATION_PROT], p0, p1); //in the future we are going to have to change this, using hardcoded strings is not good

	return result;
}
