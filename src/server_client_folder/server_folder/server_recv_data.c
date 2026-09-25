#include "server_send_recv_data.h"
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

				return rErrHand_result;

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

	if((int)header_bytes ==HARD_SHUTDOWN || (int)header_bytes == SOFT_SHUTDOWN){

		return  header_bytes;

	if(header_bytes< (ssize_t)sizeof(net_len)){

		snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_network_recv_send[RECV_LEN]);

		return -1;

	}

	uint32_t payload_len = ntohl(net_len);

	if((ssize_t)payload_len >= max_buf_len){

		snprintf(buffer_error, BUFFER_SIZE,"%s",error_string_network_recv_send[MESS_RECV_LEN_OVF]);

		return -1;

	}


	ssize_t payload_bytes = read_all(fd, buffer_message,buffer_error, (ssize_t)payload_len);

	if((int)payload_bytes == SOFT_SHUTDOWN || (int)payload_bytese == HARD_SHUTDOWN){

		return payload_bytes;

	}

	if(payload_bytes < (ssize_t)payload_len){

		snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_recv_send[RECV_LEN]);

		return -1;
	}

	buffer_message[payload_len]='\0';

	return (ssize_t)payload_bytes;

}
