#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

void setupConnection(int* client_file_descriptor,struct sockaddr_in* server_address, int port);
int send_data_to_server(char* buffer_r,char* buffer_s , int client_file_descriptor);
void handleServerCommunication(int server_port);
