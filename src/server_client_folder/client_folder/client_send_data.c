#include "client_send_data.h"

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

	return total_length;

}


ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length){

	//length is the size of the message that we want to send, excluding the null terminator the actual message that we want to send

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

	struct timeval timeout;

	timeout.tv_sec = 5;

	timeout.tv_usec = 0;


	if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1){

		printf("Error on setting timeout on client socket: %s\n", strerror(errno));

		return -1;

	}


	ssize_t result = send_all(fd, (const char *)&net_len, sizeof(net_len));

	if(result == -1){

		printf("Error, possible broken connection: %s\n", strerror(errno));

		return -1;

	}

	if ( result != sizeof(net_len)) {

        	return -1;

	}

	result = send_all(fd, payload, payload_len);

	timeout.tv_sec = 0;

	timeout.tv_usec = 0;

	if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1){

		printf("Error on setting timeout on client socket to zero: %s\n", strerror(errno));

		return -1;

	}


	if(result == -1){

		printf("Error, possible broken connection: %s:", strerror(errno));

		return -1;

	}

	if (result != (ssize_t)payload_len) {

		return -1;

	}

    return (ssize_t)(sizeof(net_len)) + (ssize_t)payload_len;
}


ssize_t receive_framed_message(int fd, char* buf, ssize_t max_buf_len){

	uint32_t net_len = 0;

	ssize_t header_bytes = 0;

	struct timeval timout;

	timeout.tv_sec = 5;

	timeout.tv_usec = 0;

	if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))==-1){

		printf("Error on setting the timer on client socket: %s\n", strerror(errno));

		return -1;

	}

	header_bytes = read_all(fd, (char*)&net_len, sizeof(net_len));

	if(header_bytes == 0){

		printf("Error: No bytes will be received: %s\n", strerror(errno));

		return -1;

	}

	if(header_bytes == -1){

		printf("Error, possible broken connection: %s\n", strerror(errno));

		return -1;

	}

	if(header_bytes< (ssize_t)sizeof(net_len)){

		printf("Error on message length\n");

		return -1;

	}


	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		printf("Error, message receiving is larger than the max length of the buffer\n");

		return -1;

	}


	ssize_t payload_bytes = read_all(fd, buf,(ssize_t)payload_len);

	timeout.tv_sec = 0;

	timeout.tv_usec = 0;

	if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))==-1){

		printf("Error on seting the timer on client socket: %s\n", strerror(errno));

		return -1;

	}

	if(payload_bytes == -1){

		printf("Error, possible broken connection: %s\n", strerror(errno));

		return -1;

	}

	if(payload_bytes < (ssize_t)payload_len){

		printf("Error, message recevied is not equal to message length recevied before in first message\n");

		return -1;
	}

	buf[payload_len]='\0';

	return (ssize_t)payload_len;

}
