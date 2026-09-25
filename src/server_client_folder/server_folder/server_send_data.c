#include "server_send_data.h"
#include "server_send_data.h"

ssize_t send_all(int temporary_fd, const char*  buffer, char* buffer_error, ssize_t length){

        ssize_t total_length  = 0, n = 0, ret  =-1;

	struct pollfd pfd;

	int pError_result = -1, pError_send = -1;

	pfd.fd = temporary_fd;

	pfd.events = (POLLOUT | POLLRDHUP);

	pfd.revents = 0;

        while(total_length<length){

		ret = poll(&pfd, 1, TM_EXP_POLL);

		pError_result = handle_poll_error(&pfd, ret , POLLOUT);

		if(pError_result != NO_SHUTDOWN){

			return pError_result;

		}

                n = send(temporary_fd,buffer+total_length,length - total_length,0);

                if(n == 0){

			break;

		}

		if(n<0){

			pError_send = error_handler_recv_send(n);

			if(pError_send != NO_SHUTDOWN){

				return pError_send;

			}

			continue;

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
