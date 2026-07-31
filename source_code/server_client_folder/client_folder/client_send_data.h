#pragma once


#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 64


int send_all(int temporary_fd, const char*  buffer, int length);

int read_all(int temporary_fd, char buffer[], int length);

int send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor);
