#include "client_send_data.h"

int send_all(int temporary_fd, const char*  buffer, int length){

        int total_length  = 0;

        int bytes_left = length;

        int n = 0;


        while(total_length<length){

                n = send(temporary_fd,buffer+total_length,bytes_left,0);

                if(n == -1){

                        break;

                }

                bytes_left-=n;

                total_length+=n;

        }


        if(n == -1){

                return bytes_left;

        }

        return total_length;

}

int read_all(int temporary_fd, char buffer[], int length){

        int total_length = 0;

        int bytes_left = length;

        int n = 0;


        while(total_length<length){


                n = read(temporary_fd, buffer+total_length,bytes_left);

                if(n == -1){

                        break;
                }


                total_length+=n;

                bytes_left-=n;

        }



        buffer[total_length] = '\0';

        return total_length;

}


int send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor){

        int bytes_sent = 0;

        int bytes_read = 0;


        int bytes_sent_client = 0;


        bytes_sent = send_all(client_file_descriptor, buffer_s,BUFFER_SIZE);


        bytes_read =  read_all(client_file_descriptor,buffer_r, BUFFER_SIZE-1);


        return bytes_read;

}
