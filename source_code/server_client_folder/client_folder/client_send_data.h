#pragma once


#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 64


size_t send_all(int temporary_fd, const char*  buffer, size_t length);

size_t read_all(int temporary_fd, char buffer[], size_t length);

size_t send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor);
