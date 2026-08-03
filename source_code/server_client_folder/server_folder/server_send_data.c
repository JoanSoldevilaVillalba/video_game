#include "server_send_data.h"

size_t read_all(int temporary_fd, char buffer[], ssize_t length){

        ssize_t total_length  = 0;

	size_t n = 0;

        while(length){

                n = read(temporary_fd,buffer+total_length,legnth - total_length);

		if(n<0){


			if(errno == EINTR)
				continue;

			return -1;

		}

		if(n == 0)
			break;

                total_length+=n;

        }

	if(total_length<length){

		buffer[total_length] = '\0';

	}else{

		buffer[length-1] = '\0';

	}


	return total_length;

}


size_t send_all(int temporary_fd, const char*  buffer, size_t length){

        int total_length  = 0;

        size_t n = 0;


        while(total_length<length){

                n = send(temporary_fd,buffer+total_length,length - total_length,0);

		if(n<0){

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
