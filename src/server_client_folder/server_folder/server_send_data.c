#include "server_send_data.h"

int handle_poll_error(pfd* pstructure_pointer, int return_value_poll, short correct_event){

	int result = -1;

	if(return_value_poll>0){

		switch(pstructure_pointer->revents){

			case POLLRDHUP:

				snprintf(buffer_error, BUFFER_SIZE; "%s", error_string_holder[]);

				break;

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

			result = 0;

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

		default:

			//nothing really happned, we have received a number of bytes
			result = result_receive;

			break;

	}

	return result;

}
ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error  , ssize_t length){

	int ret = -1, pErrHand_result = -1, rErrHand_result = -1;

        ssize_t total_length  = 0, n = 0;

	struct pollfd pfd;


	pfd.fd = temporary_fd;

	pfd.events = (POLLIN || POLLRDHUP);

	pfd.revents = 0;

        while(total_length < length){

		ret = poll(&pfd, 1, TM_EXP_POLL);

		//the following call function for now does make that much sense for the following reason:

		/*we have set that the event for inturrption is POLLIN, no other events can cause the thread to be unblocked*/

		pErrHand_result = handle_poll_error(&pfd, ret, POLLIN);

		if(pErrHand_result == -1){

			return -1;

		}

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);

		if(n == 0){

			break;

		}

		rErrHand_result = error_handler_recv(n);

		if(rErrHand_result==-1){

			return -1;

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
