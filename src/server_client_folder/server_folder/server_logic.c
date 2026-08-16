#include "server_logic.h"
#include "server_send_data.h"

void reverse(char* pointer){

        ssize_t length = strlen(pointer);

        for(ssize_t i = 0;i<length/2;i++){

                char temp = *(pointer + i);

                *(pointer + i) = *(pointer + length - 1 - i);

                *(pointer + length - 1 - i) = temp;

        }

}

//number_to_char is going to be obslete for now, ai has given us a better and safer version of the smae thing, int_to_str function
/*int number_to_char(char* pointer, int temporary){

        int i = 0;

        int residual = 0;


        while(temporary>=10){

                residual = temporary % 10;

                temporary = temporary/10;

                *(pointer + i) = residual +'0';

                i++;

        }

        *(pointer + i) = temporary + '0';

        ++i;

        *(pointer + i)  = '\0';

        reverse(pointer);

        i = i - 1;

        return i;

}
*/
int int_to_str(char *dst, ssize_t dst_size, int value){

	if(!dst||dst_size == 0){

		 return -1;
	}

	int n = snprintf(dst, dst_size, "%d", value);

	if(n<0 || (size_t)n>=dst_size){

		return -1;

	}

	return n;

}


char* create_game(int temporary_fd, char* buffer_receive, int* result_function, int* index_game, game_struct_players* game_list){

        int i = 0;

        for(i;i<MAX_GAMES_SIZE;i++){

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


void eliminate_game_slot(void* arg, int index_game, int index_player){

	struct_client fast_pointer =(struct_client*)arg;

	if(!fast_pointer){

		return;

	}

	if (!client->pointer_list_game) return;
	if (index_game < 0 || index_game >= MAX_GAMES_SIZE) return;
	if (index_player < 0 || index_player > 1) return;

	pthread_mutex_lock(&mutex_game_list);

	((fast_pointer->game_struct_players->pointer_list_game) + index_game)->player_id[index_player & 1] = -1;

	((fast_pointer->game_struct_players->pointer_list_game) + index_game)->player_ready[index_player & 1] = false;

	if(((fast_pointer->game_struct_players->pointer_list_game) + index_game)->player_id[index_player ^ 1] == -1){

		((fast_pointer->game_struct_players->pointer_list_game) + index_game)->game_id = -1;


	}

	pthread_cond_signal(&gl[index_game].game_condition);

	pthread_mutex_unlock(&mutex_game_list);

}
