#include "client_logic.h"


int menu_fd_parser(char* pointer_string, int* counter, char end_terminator){

        int i = *(counter);

        int current = 0;

        int temporary = 0;

        while(*(pointer_string + i) != end_terminator){

                temporary = *(pointer_string + i) - '0';

                current = current * 10;

                current = current + temporary;

                i++;

        }

        *(counter) = ++i;

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
