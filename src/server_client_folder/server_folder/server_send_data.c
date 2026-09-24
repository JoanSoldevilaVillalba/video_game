#include "server_send_data.h"

//we could probably add some type of special event as a parameter to indicate if we want it to be true to read or write, we will see
int handle_poll_error(pfd* pstructure_pointer, int return_value_poll, short correct_event){

	int result = -1;

	if(return_value_poll>0){

		switch(pstructure_pointer->revents){

			case POLLHUP:

				snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[SYS_POLL]);

				break;

			case POLLERR:

				snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[SYS_POLL]);

				break;

			case POLLNVAL:

				snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[SYS_POLL]);

				break;

			case correct_event:

				snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[SYS_POLL]);

				result = 0;

				break;

		}


	}else if(return_value_poll == -1){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[ERRNO_VALUES]);

	}else if(return_value_poll == 0){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[TIME_EXP_POLL]);

	}


	return result;


}
int error_handler_recv(int result_receive){

	int result = -1;

	switch(result_receive){

		case EAGAIN || EWOULDBLOCK:

			//in this case we have set the socket to non blocking for example but we have called recv, this occurs when recv does not have any infomration and allows for asycronyze programming.
			//in our case we are not setting it to non blocking but we are just goin g to have it enabled

			result = 0; //we can continue to execute

			break;

		case EBDAF:

			snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[]);

			break;

		case  EINVAL:

			snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[]);

			break;


		case ECONNREFUSED:

			snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[]);

			break;

		case ENOTCONN:

			snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[]);

			//we are in tcp, and we have not already established the connection


			break;

	}

	return result;

}
ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error  , ssize_t length){

        ssize_t total_length  = 0;

	ssize_t n = 0;

	struct pollfd pfd;


	pfd.fd = temporary_fd;

	pfd.events = POLLIN;

	pfd.revents = 0;

	int ret = -1;

        while(total_length < length){

		ret = poll(&pfd, 1, TM_EXP_POLL);

		int ret = handle_poll_error(&pfd, ret, POLLIN);

		if(ret == -1){

			return -1;

		}

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);
/*
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
*/
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

        while(total_length<length){

		ret = poll(&pfd, 1, TM_EXP_POLL);

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
