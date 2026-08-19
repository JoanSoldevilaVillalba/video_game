#include "client_main.h"


int menu_fd_parser(char* pointer_string, int* counter, char end_terminator);

int setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port);

void printMenuSC();

bool validate_message_length(const char* temporary_pointer);

ssize_t send_validated_message(const char* temporary_pointer, char buffer[BUFFER_SIZE], int client_file_descriptor);
