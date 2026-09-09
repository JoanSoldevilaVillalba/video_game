#define TEST_SERVER_LOGIC_H
#ifndef TEST_SERVER_LOGIC_H

extern const char* test_string_holder[];

bool validate_message_length(const char* temporary_pointer, char* buffer_error);

bool check_structure(char* buffer_message, char*buffer_error);

bool check_numbers(char* buffer_message, char* buffer_error);


ssize_t read_all(int temporary_fd, char buffer[], ssize_t length, char* buffer_error);

ssize_t send_all(int temporary_fd, const char*  buffer, ssize_t length, char* buffer_error);

int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port, char* buffer_error);



ssize_t receive_framed_message(int fd, char* buf, ssize_t max_buf_len, char* buffer_error);

int receive_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor);




ssize_t send_framed_message(int fd,char* buffer_message , uint32_t payload_len, char* buffer_error);

ssize_t send_validated_message(char* buffer_message, char* buffer_error, int client_file_descriptor);


#endif //TEST_SERVER_LOGIC_H
