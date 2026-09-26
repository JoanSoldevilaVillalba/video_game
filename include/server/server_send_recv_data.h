#ifndef SERVER_SEND_RECV_DATA_H
#define SERVER_SEND_RECV_DATA_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include "server.h"

#define NO_SHUTDOWN 1
#define SOFT_SHUTDOWN -1
#define HARD_SHUTDOWN -2
#define TIMED_OUT -3

typedef enum {
	SYS_POLL,
	POLL_RDHUP,
	POLL_HUP,
	POLL_ERR,
	POLL_NVAL,
	POLL_IN,
	ERRNO_VALUES_POLL,
	TIME_EXP_POLL,
	INVALID_FD_RECV,
	INVALID_ARG_RECV,
	NO_SETUP_RECV,
	NO_SPECIFIC_ERROR_RECV,
	RECV_LEN,
	MESS_RECV_LEN_OVF,
	SEND_LEN
} error_network_id;

extern const char* error_string_network_holder[];
extern const char* error_string_network_recv_send[];

ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error, ssize_t length);
ssize_t send_all(int temporary_fd, const char* buffer, char* buffer_error, ssize_t length);
ssize_t send_framed_message(int fd, const char* payload, char* buffer_error, uint32_t payload_len);
ssize_t receive_framed_message(int fd, char* buf, char* buffer_error, ssize_t max_buf_len);

int handle_poll_error(struct pollfd* pstructure_pointer, int return_value_poll, short expected, char* buffer_error);
int error_handler_recv_send(int result_receive, char* buffer_error);

#endif //SERVER_SEND_RECV_DATA_H
