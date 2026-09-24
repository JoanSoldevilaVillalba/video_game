#include "server_send_data.h"
#include "server_send_data.h"

const char* error_string_network_poll[] = {
[SYS_POLL] = "poll(): an error event was detected",
[POLL_RDHUP] = "poll(): peer performed a half-close (POLLRDHUP)",
[POLL_HUP] = "poll(): connection hangup detected (POLLHUP)",
[POLL_ERR] = "poll(): error condition detected (POLLERR)",
[POLL_NVAL] = "poll(): invalid file descriptor (POLLNVAL)",
[POLL_IN] = "poll(): data is available to read (POLLIN)",
[ERRNO_VALUES_POLL] = "poll(): system call failed; check errno",
[TIME_EXP_POLL] ="poll(): timeout expired; no events occurred"
};

const char* error_string_network_recv[]={
[INVALID_FD_RECV] = "Invalid file descriptor is being used for current thread connection",
[INVALID_ARG_RECV] = "Invalid argument was passed to recv sys call",
[NO_SETUP_RECV] = "Connection was not established with the client"
};

const char* error_string_network_send[]={

};

int handle_poll_error(pfd* pstructure_pointer, int return_value_poll){

	int result = -1;

	if(return_value_poll>0){


		switch (pstructure_pointer->revents) {

			case POLLRDHUP:

				snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_RDHUP]);

				result = SOFT_SHUTDOWN;

			break;

			case POLLHUP:

				snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_HUP]);

				/*en realitzar la crida sincrona, el sistema operatiu, el seu packet network 
manager, veu que estem esperant per un packet duna connexio que ja s'havia tancat. De manera 
que sense haver de rebre cap paquet, el network manager respon al nostre programma indicant que 
la connexio socket ja esta desconnectada*/

				result = HARD_SHUTDOWN;

				break;

			case POLLERR:

				//this error can occur due to asyncronus reasons:

				/*
				when calling poll function, our program is invoking the operating system to perform a system call, puttuing our thread to 
				sleep untill an event occurs. The operating system then hands these to the network packet manager, which receives an error for example a RST packet,
				*/

				snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_ERR]);

				result = HARD_SHUTDOWN;

				break;

			case POLLNVAL:

				//invalid request, meaning the file descriptor is not pointing to an actual file

				snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_NVAL]);

				result = HARD_SHUTDOWN;

				break;

			case POLLIN:

				//this is the correct case, no error, therfore NO_SHUTDOWN

				snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_IN]);

				result = NO_SHUTDOWN;

				break;

			}

	}else if(return_value_poll == -1){

		//errno error, this is not a connection problem, or an abstract socket problem, but rather the actual poll procedure failed due to memory space or something else

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_network_poll[ERRNO_VALUES]);

		result = HARD_SHUTDOWN;


	}else if(return_value_poll == 0){

		//time expired, client failed to send a message to use (constant packae drop for example)

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_netowrk_poll[TIME_EXP_POLL]);

		result = SOFT_SHUTDOWN;

	}


	return result;


}
int error_handler_recv(int result_receive){

	int result = -1;

	switch(result_receive){

		case EAGAIN || EWOULDBLOCK:

			result = 0;

			result = NO_SHUTDOWN;

			break;

		case EBDAF:

			snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_recv[]);

			//using an invalid file descriptor: negative integer, not initialized or opened befor

			result = HARD_SHUTDOWN;

			break;

		case  EINVAL:

			snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[]);

			//we have passed an invalid argument to recv

			result = HARD_SHUTDOWN;

			break;

		case ENOTCONN:

			//no connection has been done to the file descriptor/socket
			//this one will probably never happen b

			snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[]);

			result = HARD_SHUTDOWN;

			break;

			//we are in tcp, and we have not already established the connection

		default:

			result = NO_SHUTDOWN;

			break;

	}

	return result;

}
ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error  , ssize_t length){

	int ret = -1, pErrHand_result = -1, rErrHand_result = -1;

        ssize_t total_length  = 0, n = 0;

	struct pollfd pfd;


	pfd.fd = temporary_fd;

	pfd.events = (POLLIN | POLLRDHUP);

	pfd.revents = 0;

        while(total_length < length){

		ret = poll(&pfd, 1, TM_EXP_POLL);

		pErrHand_result = handle_poll_error(&pfd, ret, POLLIN);

		if(pErrHand_result != NO_SHUTDOWN){

			return pErrHand_result;

		}

                n = recv(temporary_fd,buffer+total_length,length - total_length, 0);

		if(n == 0){

			break;

		}

		rErrHand_result = error_handler_recv(n);

		if(rErrHand_result != NO_SHUTDOWN){

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
	//i don't think the following condition will ever happen, due to the experation,the socket will keep waiting untill all bytes asked are received, or when the time expeires, when time expeires a negative -1 is returned
	if(header_bytes< (ssize_t)sizeof(net_len)){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_holder[INIT_RECV_LEN]);

		return -1;

	}

	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		//we need to figure out what to do here still, because, the client is still going to try to send the real message, regardless if there is buffer over_flow (even thought the client is going to be programmed in order to prevent this aswell, safeguards are necessary but maybe this one is not necessary i don't know)

		snprintf(buffer_error, BUFFER_SIZE,"%s",error_string_holder[MESS_RECV_LEN_OVF]);

		return -1;

	}


	ssize_t payload_bytes = read_all(fd, buffer_message,buffer_error, (ssize_t)payload_len);

	if(payload_bytes<=0){

		return -1;

	}

	if(payload_bytes < (ssize_t)payload_len){

		snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_holder[MESS_RECV_LEN]);

		return -1;
	}

	buffer_message[payload_len]='\0';

	return (ssize_t)payload_bytes;

}
