#include "server_send_recv_data.h"

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

const char* error_string_network_recv_send[]={
[INVALID_FD_RECV] = "recv: Invalid file descriptor is being used for current thread connection",
[INVALID_ARG_RECV] = "recv: Invalid argument was passed to recv sys call",
[NO_SETUP_RECV] = "recv: Connection was not established with the client",
[NO_SPECIFIC_ERROR_RECV] = "recv: error happend but could not find specific error",

[RECV_LEN] = "Received message len incorrect",
[MESS_RECV_LEN_OVF] = "Received message is overflowing buffer",

[SEND_LEN] = "Sent message length is incorrect"

};

int handle_poll_error(pfd* pstructure_pointer, int return_value_poll, short expected,char*buffer_error){

        int result = -1;

        if(return_value_poll>0){

                        if(pstructure_pointer->revents & POLLRDHUP){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_RDHUP]);

                                result = SOFT_SHUTDOWN;

                        }else if(pstructure_pointer->revents & POLLHUP){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_HUP]);

                                result = HARD_SHUTDOWN;

                        }else if(pstructure_pointer->revents & POLLERR){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_ERR]);

                                result = HARD_SHUTDOWN;

                        }else if(pstructure_pointer->revents & POLLNVAL){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_NVAL]);

                                result = HARD_SHUTDOWN;

                        }else if((pstructure_pointer->revents & POLLIN) && (pstructure_pointer->revents & expected)){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_IN]);

                                result = NO_SHUTDOWN;

                        }else if((pstructure_pointer->revents & POLLOUT) && (pstructure_pointer->revents & expected)){


				snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_network_poll[POLL_IN]);

				result = NO_SHUTDOWN;

			}

        }else if(return_value_poll == -1){

                snprintf(buffer_error, BUFFER_SIZE, error_string_network_poll[ERRNO_VALUES], strerror(errno));

                result = HARD_SHUTDOWN;


        }else if(return_value_poll == 0){

                snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_netowrk_poll[TIME_EXP_POLL]);

                result = SOFT_SHUTDOWN;

        }


        return result;


}
int error_handler_recv_send(int result_receive,char*buffer_error){

	int result = 0;

                if(errno == EAGAIN || erno == EWUOLDBLOCK || errno ==EINTR){

                        result = NO_SHUTDOWN;

                }else{

                        switch(result_receive){

                                case EBDAF:

                                        snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_recv_send[INVALID_FD_RECV]);

                                        result = HARD_SHUTDOWN;

                                        break;

                                case  EINVAL:

                                        snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_recv_send[INVALID_ARG_RECV]);

                                        result = HARD_SHUTDOWN;

                                        break;

                                case ENOTCONN:

                                        snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_network_recv_send[NO_SETUP_RECV]);

                                        result = HARD_SHUTDOWN;

                                        break;

                                default:

                                        result = HARD_SHUTDOWN;

                                        snprintf(buffer_error, BUFFE_SIZE, "%s", error_string_network_recv_send[NO_SPECIFIC_ERROR_RECV]);

                                        break;

                                }

                        }


        return result;

}
