
#ifndef TEST_SERVER_1_H

#define TEST_SERVER_1_H



#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>


#define BUFFER_SIZE 64

typedef enum{

TEST_CONNECTION_FD_ERROR = 0,
TEST_CONNECTION_FRMT_ERROR = 1,
TEST_CONNECTION_CNT_ERROR = 2,
TEST_NULL_POINTER_ERROR = 3,
TEST_SEND_TIMEOUT_ERROR = 4,
TEST_SEND_RESET_TMO_ERROR = 5,
TEST_SEND_LENGTH_INIT_ERROR = 6,
TEST_SEND_LENGTH_MESS_ERROR = 7,
TEST_MESSAGE_LENGTH_ERROR = 8,
TEST_SEND_SYS_CALL_ERROR = 9,
TEST_RECV_VALD_STRUCT_FRST_ERROR = 10,
TEST_RECV_VALD_STRUCT_SCND_ERROR = 11,
TEST_RECV_VALD_NN_FRST_ERROR = 12,
TEST_RECV_VALD_NN_SCND_ERROR = 13,
TEST_RECV_VALD_VALUE_FRST_ERROR = 14,
TEST_RECV_VALD_VALUE_SCND_ERROR = 15,
TEST_RECV_SYSCALL_ERROR = 16,
TEST_RECV_TIMEOUT_ERROR = 17,
TEST_RECV_RESET_TMO_ERROR= 18,
TEST_RECV_NO_BYTES_ERROR = 19,
TEST_RECV_INIT_LENGTH_ERROR = 20,
TEST_RECV_MESS_LENGTH_ERROR = 21,
TEST_RECV_MAX_LENGTH_ERROR = 22

}testID;

typedef enum{

RANDOM_MESSAGE_TEST = 0,
QUIT_CLIENT_MESSAGE_TEST = 1

}message_server_id;



extern const char* test_string_holder[];
extern const char* test_message_server[];


int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port, char* buffer_error);

int test_setup_connection(char* buffer_error, int server_port);

bool validate_message_length(const char* temporary_pointer, char* buffer_error);

ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length, char* buffer_error);

ssize_t send_framed_message(int fd,char* buffer_message , uint32_t payload_len, char* buffer_error);

ssize_t send_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor);

int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor);

#endif //TEST_SERVER_1_H
