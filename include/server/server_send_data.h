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

ssize_t read_all(int temporary_fd, char* buffer, char* buffer_error, ssize_t length);

ssize_t send_all(int temporary_fd, const char*  buffer, char* buffer_error, ssize_t length);

ssize_t send_framed_message(int fd, const char *payload, char* buffer_error, uint32_t payload_len);

ssize_t receive_framed_message(int fd, char* buf, char* buffer_error, ssize_t max_buf_len);

#endif SERVER_SEND_DATA
