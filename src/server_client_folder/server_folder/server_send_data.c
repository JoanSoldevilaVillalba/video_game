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

			snprintf(buffer_error, BUFFER_SIZE, error_string_holder[SYS_POLL],strerror(errno));

			return -1;

		}else if(ret == 0){

			snprintf(buffer_error,BUFFER_SIZE, "%s" ,error_string_holder[TIME_EXPIRED_POLL]);

			return -1;

		}

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);

		if(n<0){


			if(errno == EINTR){

				continue;
			}

			snprintf(buffer_error , BUFFER_SIZE,error_string_holder[SYS_RECV],strerror(errno)); //strerror(errno);

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

			snprintf(buffer_error,BUFFER_SIZE,error_string_holder[SYS_POLL], strerror(errno)); //strerror(errno);

			return -1;

		}else if(ret == 0){


			snprintf(buffer_error, BUFFER_SIZE,"%s",error_string_holder[TIME_EXPIRED_POLL]);

			return -1;

		}


                n = send(temporary_fd,buffer+total_length,length - total_length,0);

		if(n==-1){

			if(errno == EINTR){

				continue;

			}

			snprintf(buffer_error, BUFFER_SIZE,error_string_holder[SYS_SEND], strerror(errno));

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

	ssize_t result = send_all(fd, (const char *)&net_len, buffer_error, sizeof(net_len));

	if((int)result == -1){

		return -1;

	}


	if ( result != sizeof(net_len)) {

		snprintf(buffer_error,BUFFER_SIZE,"%s", error_string_holder[INIT_SEND_LEN]);

        	return -1;
	}

	result = send_all(fd, payload, buffer_error,payload_len);

	if((int)result == -1){


		return -1;

	}

	if ((int)result != (ssize_t)payload_len) {

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[MESS_SEND_LEN]);

	        return -1;
	}

    return sizeof(net_len) + payload_len;
}


ssize_t receive_framed_message(int fd, char* buffer_message, char* buffer_error, ssize_t max_buf_len){

	uint32_t net_len = 0;

	ssize_t header_bytes = 0;

	header_bytes = read_all(fd, (char*)&net_len, buffer_error, sizeof(net_len));

	if((int)header_bytes <= 0){

		return -1;

	}

	if(header_bytes< (ssize_t)sizeof(net_len)){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[INIT_RECV_LEN]);

		return -1;

	}

	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		snprintf(buffer_error, BUFFER_SIZE,"%s",error_string_holder[MESS_RECV_LEN_OVF]);

		return -1;

	}


	ssize_t payload_bytes = read_all(fd, buffer_message,buffer_error, (ssize_t)payload_len);

	if(payload_bytes<0){

		return -1;

	}

	if(payload_bytes < (ssize_t)payload_len){

		snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[MESS_RECV_LEN]);

		return -1;
	}

	buffer_message[payload_len]='\0';

	return (ssize_t)payload_bytes;

}
