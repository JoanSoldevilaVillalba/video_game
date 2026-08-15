#include "server_logic.h"
#include "server.h"
void reverse(char* pointer){

        int length = strlen(pointer);

        for(int i = 0;i<length;i++){

                char temp = *(pointer + i);

                *(pointer + i) = *(pointer + length - 1 - i);

                *(pointer + length - 1 - i) = temp;

        }

}


int number_to_char(char* pointer, int temporary){

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

char* create_game(int temporary_fd, char* buffer_receive, int* result_function, int* index_game, game_struct_players* game_list){

        int i = 0;

        for(i;i<MAX_GAMES_SIZE;i++){

                pthread_mutex_lock(&mutex_game_list);

                if((game_list+i)->game_id!=-1 && (game_list+i)->second_player==-1){

                        (game_list+i)->second_player = temporary_fd;
                        *(result_function) = 1;
                        *(index_game) = i;
                        pthread_cond_signal(&(game_list + i)->game_condition);
                        pthread_mutex_unlock(&mutex_game_list);
                        return "0|1|you have succesfuly entered a game";

                }else if((game_list+i)->game_id==-1){

                        (game_list+i)->game_id = i;
                        (game_list+i)->first_player = temporary_fd;
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

                        printf("Unable to send message to server: %s\n", strerror(errno));

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
