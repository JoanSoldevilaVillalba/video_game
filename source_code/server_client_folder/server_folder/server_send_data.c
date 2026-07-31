#include "server_send_data.h"

int read_all(int temporary_fd, char buffer[], int length){

        int total_length  = 0;

        int bytes_left = length;

        int n = 0;


        while(total_length<length){

                n = read(temporary_fd,buffer+total_length,bytes_left);

                if(n == -1){


                        printf("Error,the tcp connection was lost\n");

                        break;

                }

                bytes_left-=n;

                total_length+=n;

        }


        if(n == -1){

                return bytes_left;

        }


        buffer[total_length] = '\0';

        return total_length;

}


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
