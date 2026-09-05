
#ifndef TEST_SERVER_1_H

#define TEST_SERVER_1_H



#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>


#define BUFFER_SIZE 64

typedef enum testID;

extern const char* test_string_holder[];

int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port, char* buffer_error);


#endif //TEST_SERVER_1_H
