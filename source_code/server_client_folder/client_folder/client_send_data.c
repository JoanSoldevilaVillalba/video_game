#include "client_send_data.h"

size_t send_all(int temporary_fd, const char*  buffer, size_t length){

        size_t total_length  = 0;

        size_t n = 0;


        while(total_length<length){

                n = send(temporary_fd,buffer+total_length,length - total_length,0);

                if(n < 0 ){

			if(errno == EINTR){

				continue;

			}
			return -1;


                }

		if(n == 0){

			break;

		}


                total_length+=n;

        }

        return total_length;

}

size_t read_all(int temporary_fd, char buffer[], size_t length){

        int total_length = 0;

        size_t n = 0;

        while(total_length<length){

                n = read(temporary_fd, buffer+total_length,length-total_length);

                if(n < 0){

			if(errno == EINTR){

				continue;

			}

                        break;
                }


		if(n == 0){

			break;

		}


                total_length+=n;

        }



	if(total_length<length){

        buffer[total_length] = '\0';

	}else{

		buffer[total_length] == '\0';

	}

        return total_length;

}


size_t send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor){

        size_t bytes_sent = 0;

        size_t bytes_read = 0;

        bytes_sent = send_all(client_file_descriptor, buffer_s,BUFFER_SIZE);


        bytes_read =  read_all(client_file_descriptor,buffer_r, BUFFER_SIZE-1);


        return bytes_read;

}
