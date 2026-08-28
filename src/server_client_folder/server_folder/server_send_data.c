#include "server_send_data.h"


//in the future we are going to craete a new type of struct, called Error, in this struct we are going to allocate a char buffer aswell as a errno error number, and maybe we can also add a field for strerror(errno)

ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error  , ssize_t length){

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

			//strcpy(buffer_error, "Error in event driven poll syscall\0");//strerror(errno);

			snprintf(buffer_error, sizeof(buffer_error), "%s", "Error in event driven poll syscall");

			return -1;

		}else if(ret == 0){

			snprintf(buffer_error,sizeof(buffer_error), "%s" ,"Error, time out expired for poll event");

			return -1;

		}

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);

		if(n<0){


			if(errno == EINTR){

				continue;
			}

			snprintf(buffer_error ,sizeof(buffer_error),"%s","syscall error (recv syscall)"); //strerror(errno);

			return -1;

		}

		if(n == 0){

			break;

		}
                total_length+=n;

        }

	return total_length;

}


ssize_t send_all(int temporary_fd, const char*  buffer, char* buffer_error, ssize_t length){

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

			snprintf(buffer_error,sizeof(buffer_error), "%s","Error, in event driven poll syscall"); //strerror(errno);

			return -1;

		}else if(ret == 0){


			snprintf(buffer_error, sizeof(buffer_error),"%s","Error, time out expired for poll event");

			return -1;

		}


                n = send(temporary_fd,buffer+total_length,length - total_length,0);

		if(n==-1){

			if(errno == EINTR){

				//system call was inturrpted, no error actually occurred

				continue;

			}

			snprintf(buffer_error, sizeof(buffer_error),"%s", "Error, possible broken connection");
			return -1;

		}

               if(n == 0){

			break;

		}

                total_length+=n;

        }


        return total_length;

}
ssize_t send_framed_message(int fd, const char *payload, char* buffer_error, uint32_t payload_len) {

	uint32_t net_len = htonl(payload_len);

	size_t result = send_all(fd, (const char *)&net_len, buffer_error, sizeof(net_len));

	if((int)result == -1){

		return -1;

	}


	if ( result != sizeof(net_len)) {

		snprintf(buffer_error,sizeof(buffer_error),"%s", "Error, amount of bytes sent is not equal to its protocol theoretical value (initilae message)");

        	return -1;
	}

	result = send_all(fd, payload, buffer_error,payload_len);

	if((int)result == -1){


		return -1;

	}

	if ((int)result != (ssize_t)payload_len) {

		snprintf(buffer_error, sizeof(buffer_error), "%s", "Error, amount of bytes sent is not equal to its protocol theoretical value (actual message)");

	        return -1;
	}

    return sizeof(net_len) + payload_len;
}


ssize_t receive_framed_message(int fd, char* buffer_message, char* buffer_error, ssize_t max_buf_len){

	uint32_t net_len = 0;

	ssize_t header_bytes = 0;

	header_bytes = read_all(fd, (char*)&net_len, buffer_error, sizeof(net_len));

	if((int)header_bytes == 0){

		return -1;

	}

	if(header_bytes< (ssize_t)sizeof(net_len)){

		snprintf(buffer_error, sizeof(buffer_error), "%s", "Error, we where not able to receive the 4 bytes containing the length of the message");

		return -1;

	}

	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		snprintf(buffer_error, sizeof(buffer_error),"%s","Error, the message that we want to receive is larger than the max length of the buffer");

		return -1;

	}


	ssize_t payload_bytes = read_all(fd, buffer_message,buffer_error, (ssize_t)payload_len);

	if(payload_bytes<0){

		return -1;

	}

	if(payload_bytes < (ssize_t)payload_len){

		snprintf(buffer_error, sizeof(buffer_error), "%s","Error, the number of bytes received from the actual messages is not the same as the first 4 byte number");

		return -1;
	}

	buffer_message[payload_len]='\0';

	return (ssize_t)payload_bytes;

}
