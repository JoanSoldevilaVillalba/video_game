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
[NO_SPECIFIC_ERROR] = "recv: error happend but could not find specific error"
};

int handle_poll_error(pfd* pstructure_pointer, int return_value_poll, short expected){

        int result = -1;

        if(return_value_poll>0){

                        if(pstructure_pointer->revents & POLLRDHUP){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_RDHUP]);

                                result = SOFT_SHUTDOWN;

                        }else if(pstructure_pointer->revents & POLLHUP){

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_HUP]);
                                /*en realitzar la crida sincrona, el sistema operatiu, el seu packet network
manager, veu que estem esperant per un packet duna connexio que ja s'havia tancat. De manera
que sense haver de rebre cap paquet, el network manager respon al nostre programma indicant que
la connexio socket ja esta desconnectada*/
                                result = HARD_SHUTDOWN;

                        }else if(pstructure_pointer->revents & POLLERR){

                                //this error can occur due to asyncronus reasons:
                                /*
                                when calling poll function, our program is invoking the operating system to perform a system call, puttuing our thread to
                                sleep untill an event occurs. The operating system then hands these to the network packet manager, which receives an error for example a RST packet,
                                */

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_ERR]);

                                result = HARD_SHUTDOWN;

                        }else if(pstructure_pointer->revents & POLLNVAL){
                                //invalid request, meaning the file descriptor is not pointing to an actual file

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_NVAL]);

                                result = HARD_SHUTDOWN;

                        }else if((pstructure_pointer->revents & POLLIN) && (pstructure_pointer->revents & expected)){
                                //this is the correct case, no error, therfore NO_SHUTDOWN

                                snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_poll[POLL_IN]);

                                result = NO_SHUTDOWN;

                        }else if((pstructure_pointer->revents & POLLOUT) && (pstructure_pointer->revents & expected)){


				snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_network_poll[POLL_IN]);

				result = NO_SHUTDOWN;

			}

        }else if(return_value_poll == -1){

                //errno error, this is not a connection problem, or an abstract socket problem, but rather the actual poll procedure failed due to memory space or something else

                snprintf(buffer_error, BUFFER_SIZE, error_string_network_poll[ERRNO_VALUES], strerror(errno));

                result = HARD_SHUTDOWN;


        }else if(return_value_poll == 0){

                //time expired, client failed to send a message to use (constant packae drop for example)

                snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_netowrk_poll[TIME_EXP_POLL]);

                result = SOFT_SHUTDOWN;

        }


        return result;


}
int error_handler_recv_send(int result_receive){

        int result = 0;

        if(result_receive<0){

                if(errno == EAGAIN || erno == EWUOLDBLOCK || errno ==EINTR){

                        //EINTR, the call was inturrupted, data can still be in the buffer, we can still read.

                        result = NO_SHUTDOWN;

                }else{

                        switch(result_receive){

                                case EBDAF:

                                        snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_recv_send[INVALID_FD_RECV]);

                                        //using an invalid file descriptor: negative integer, not initialized or opened befor

                                        result = HARD_SHUTDOWN;

                                        break;

                                case  EINVAL:

                                        snprintf(buffer_error, BUFFER_SIZE, "%s",error_string_network_recv_send[INVALID_ARG_RECV]);

                                        //we have passed an invalid argument to recv

                                        result = HARD_SHUTDOWN;

                                        break;

                                case ENOTCONN:

                                        //no connection has been done to the file descriptor/socket
                                        //this one will probably never happen b

                                        snprintf(buffer_error, BUFFER_SIZE, "%s", error_string_network_recv_send[NO_SETUP_RECV]);

                                        result = HARD_SHUTDOWN;

                                        break;

                                        //we are in tcp, and we have not already established the connection

                                default:

                                        result = HARD_SHUTDOWN;

                                        snprintf(buffer_error, BUFFE_SIZE, "%s", error_string_network_recv_send[NO_SPECIFIC_ERROR_RECV]);

                                        break;

                                }

                        }


        }

        return result;

}
