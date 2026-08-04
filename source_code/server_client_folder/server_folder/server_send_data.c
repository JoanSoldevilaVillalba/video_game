#include "server_send_data.h"

ssize_t read_all(int temporary_fd, char buffer[], ssize_t length){

        ssize_t total_length  = 0;

	ssize_t n = 0;

        while(total_length < length){

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);

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

	if(total_length<length){

		buffer[total_length] = '\0';

	}else{

		buffer[length-1] = '\0';

	}


	return total_length;

}


ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length){

        ssize_t total_length  = 0;

        ssize_t n = 0;


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
ssize_t send_framed_message(int fd, const char *payload, uint32_t payload_len) {

	uint32_t net_len = htonl(payload_len);

	size_t result = send_all(fd, (const char *)&net_len, sizeof(net_len));


	if ( result != sizeof(net_len)) {
        	return -1;
	}

	result = send_all(fd, payload, payload_len);

	if (result != (ssize_t)payload_len) {
	        return -1;
	}

    return sizeof(net_len) + payload_len;
}


ssize_t receive_framed_message(int fd, char* buf, ssize_t max_buf_len){

	//primer nomes cal llegir 4 bvytes, per averiguar lo llarg que es el mmissatge que  nosaltres volem rebre realment

	uint32_t net_len = 0;

	ssize_t header_bytes = 0;

	//we are implicilty casting net_len to be treated as a const char pointer. Because we are implicilty casting it, that means that the compiler is 
	//going to have to convert the integer into a string

	header_bytes = read_all(fd, (char*)&net_len, sizeof(net_len));//we first receive the first 4 bytes that contain the length of the actual message that we are trying to receive

	if(header_bytes == 0){

	//zero bytes where sent, we can exit
		return header_bytes;

	}

	if(header_bytes <0){
	//somehting bad happened
	printf("Error, connection crashed\n");

	}

	if(header_bytes< (ssize_t)sizeof(net_len)){

		printf("Error, we where not able to receive the 4 bytes containing the length of the message\n");

		return -1;

	}


	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		printf("Error the message that we want to receive is larger than the max length of the buffer\n");

		return -1;

	}


	ssize_t payload_bytes = read_all(fd, buf,(ssize_t)payload_len);

	if(payload_bytes < (ssize_t)payload_len){

		printf("Error, the number of bytes received from the actual messages is not the same as the first 4 byte number\n");

		return -1;
	}

	buf[payload_len]='\0';

	return (ssize_t)payload_bytes;

}
