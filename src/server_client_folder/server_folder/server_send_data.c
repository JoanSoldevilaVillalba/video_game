#include "server_send_data.h"

ssize_t read_all(int temporary_fd, char buffer[], ssize_t length){

        ssize_t total_length  = 0;

	ssize_t n = 0;

	struct pollfd pfd;


	pfd.fd = temporary_fd;

	pfd.events = POLLIN;

	pfd.revents = 0;

	int ret = -1;

	int time_out = 20000;

        while(total_length < length){

		ret = poll(&pfd, 1,time_out );

		if(ret == -1){

			printf("Error in event driven poll syscall: %s\n", strerror(errno));

			return -1;

		}else if(ret == 0){

			printf("Error, time out expired for poll event\n");

			return -1;

		}

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

        ssize_t total_length  = 0;

        ssize_t n = 0;

	struct pollfd pfd;



	pfd.fd = temporary_fd;

	pfd.events = POLLOUT;

	pfd.revents = 0;

	ssize_t ret  =-1;

	int time_out = 15000;

        while(total_length<length){

		ret = poll(&pfd, 1, time_out);

		if(ret == -1){

			printf("Error in event driven poll syscall: %s\n", strerror(errno));

			return -1;

		}else if(ret == 0){

			printf("Error, time out expired for poll event\n");

			return -1;

		}


                n = send(temporary_fd,buffer+total_length,length - total_length,0);

		if(n==-1){

			if(errno == EINTR){

				//system call was inturrpted, no error actually occurred

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

	if((int)result == -1){

		printf("Error, possible broken connection\n");

		return -1;

	}


	if ( result != sizeof(net_len)) {
        	return -1;
	}

	result = send_all(fd, payload, payload_len);

	if((int)result == -1){

		printf("Error, possible broken connection\n");

	}

	if ((int)result != (ssize_t)payload_len) {
	        return -1;
	}

    return sizeof(net_len) + payload_len;
}


ssize_t receive_framed_message(int fd, char* buf, char* buffer_error, ssize_t max_buf_len){

	uint32_t net_len = 0;

	ssize_t header_bytes = 0;

	header_bytes = read_all(fd, (char*)&net_len, sizeof(net_len));

	if((int)header_bytes == 0){

		printf("Error, no bytes will be received\n");

		return -1;

	}

	if((int)header_bytes == -1){

		printf("Error, borken connection possible: %s\n", strerror(errno));

		return -1;

	}

	if(header_bytes< (ssize_t)sizeof(net_len)){

		printf("Error, we where not able to receive the 4 bytes containing the length of the message\n");

		return -1;

	}

	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		printf("Error, the message that we want to receive is larger than the max length of the buffer\n");

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
