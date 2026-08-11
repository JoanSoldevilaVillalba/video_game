#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <poll.h>

ssize_t read_all(int temporary_fd, char buffer[], ssize_t length);

ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length);

ssize_t send_framed_message(int fd, const char *payload, uint32_t payload_len);

ssize_t receive_framed_message(int fd, char* buf, ssize_t max_buf_len);
