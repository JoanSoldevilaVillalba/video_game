#include "server_send_data.h"
#include "server_send_data.h"

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

		if(n<0){

			rErrHand_result = error_handler_recv(n);

			if(rErrHand_result != NO_SHUTDOWN){

				return -1;

			}

			continue;

		}

              total_length+=n;

        }

	return total_length;

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
