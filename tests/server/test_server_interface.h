#define TEST_SERVER_INTERFACE_H
#ifndef TEST_SERVER_INTERFACE_H

extern const char* test_message_server[];

 extern const char* test_string_holder[]={

[TEST_CONNECTION_FD_ERROR] = "Error, no file descriptor was assigned to client: %s",
[TEST_CONNECTION_FRMT_ERROR] = "Error, possible incorrect format: %s",
[TEST_CONNECTION_CNT_ERROR] = "Error, connection with server was not possible: %s",


[TEST_NULL_POINTER_ERROR] = "Error, pointer is null",


[TEST_SEND_TIMEOUT_ERROR] = "Error on setting timeout on client socket when sending information: %s",
[TEST_SEND_RESET_TMO_ERROR] = "Error on resetting timeout to zero on client socket: %s",
[TEST_SEND_LENGTH_INIT_ERROR] = "Error, the amount of bytes that were sent in init message is not equal to theoretical value",
[TEST_SEND_LENGTH_MESS_ERROR] = "Error, the amount of bytes that were sent in the real message is not equal to its actual size (original message size not equal to bytes sent)",
[TEST_SEND_SYS_CALL_ERROR] = "Error, syscall error has occurred: %s",

[TEST_MESSAGE_LENGTH_ERROR] = "Error, message length is too long, overflow",

[TEST_RECV_VALD_STRUCT_FRST_ERROR] = "Error, first seperator is missing in the following message: %s",
[TEST_RECV_VALD_STRUCT_SCND_ERROR] = "Error, second seperator is missing in the following message: %s",
[TEST_RECV_VALD_NN_FRST_ERROR] = "Error, there is no first number: %s",
[TEST_RECV_VALD_NN_SCND_ERROR] = "Error, there is no second number: %s",
[TEST_RECV_VALD_VALUE_FRST_ERROR] = "Error, first value of message is not correct",
[TEST_RECV_VALD_VALUE_SCND_ERROR]= "Error, second value of message is not correct",
[TEST_RECV_SYSCALL_ERROR] = "Error, syscall error when receving bytes from server: %s",
[TEST_RECV_TIMEOUT_ERROR] = "Error on setting timeout on client socket when receving information: %s",
[TEST_RECV_RESET_TMO_ERROR]="Error on resetting timeout to zero on client socket when receving information: %s",
[TEST_RECV_NO_BYTES_ERROR] = "Error, no byte were recevied from the server",
[TEST_RECV_INIT_LENGTH_ERROR] = "Error, the amount of bytes that were recevied in init message is not equal to theoretical value",
[TEST_RECV_MESS_LENGTH_ERROR] = "Error, the amount of bytes that were received in the real message is not equal to its actual size",
[TEST_RECV_MAX_LENGTH_ERROR] = "Error, init message indicates message is to large for client, possible overflow",
};

int test_quit_client(char* buffer_message, char* buffer_error, int file_descriptor);

int test_receive_message(char* buffer_message, char* buffer_error, int file_descriptor);

int test_setup_connection(char* buffer_error, int server_port);

int test_create_game_client(char* buffer_message,char*  buffer_error,int client_file_descriptor);

int test_send_message(char* buffer_message, char* buffer_error, int client_file_descriptor);


#endif //TEST_SERVER_INTERFACE_H
