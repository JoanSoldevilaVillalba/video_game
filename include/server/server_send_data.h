#ifndef SERVER_SEND_DATA_H
#define SERVER_SEND_DATA_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include "server.h"


//from least to most aggressive (no shutdown to forced connection shutdown)

#define NO_SHUTDOWN 1

#define TIMED_OUT -3

#define SOFT_SHUTDOWN -1

#define HARD_SHUTDOWN -2


typedef enum{

	SYS_POLL,          // poll() returned an error event

	POLL_RDHUP,        // peer closed its writing side

	POLL_HUP,          // connection hangup

	POLL_ERR,          // poll error

	POLL_NVAL,         // invalid file descriptor

	POLL_IN,           // data available to read

	ERRNO_VALUES_POLL,      // poll() itself returned -1

	TIME_EXP_POLL,      // poll() timed out

	INVALID_FD_RECV,

	INVALID_ARG_RECV,

	NO_SETUP_RECV,


}error_network_id;


extern const char* error_string_network_holder[];

extern const char* error_string_network_recv[];

ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error, ssize_t length);

ssize_t send_all(int temporary_fd, const char*  buffer, char* buffer_error, ssize_t length);

ssize_t send_framed_message(int fd, const char *payload, char* buffer_error, uint32_t payload_len);

ssize_t receive_framed_message(int fd, char* buf, char* buffer_error, ssize_t max_buf_len);

#endif //SERVER_SEND_DATA_H
