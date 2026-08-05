#pragma once

/*
includes (stdint.h, arpa/inet.h, sys/types.h, errno.h, unistd.h, string.h).

*/
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

#define BUFFER_SIZE 64


ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length);

ssize_t read_all(int temporary_fd, char buffer[], ssize_t length);


ssize_t send_framed_message(int fd, const char *payload, uint32_t payload_len);

ssize_t receive_framed_message(int fd, char* buf, ssize_t max_buf_len);



//we are not really using the folloiwng function:
ssize_t send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor);
